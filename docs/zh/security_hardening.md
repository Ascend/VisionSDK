# 安全加固<a name="ZH-CN_TOPIC_0000001608030249"></a>

## 安全要求<a name="ZH-CN_TOPIC_0000001582339874"></a>

使用API读取文件时，用户需要保证该文件的owner必须为自己，且权限不大于640，避免发生提权等安全问题。

外部下载的软件代码或程序可能存在风险，功能的安全性需由用户保证。


## 加固须知<a name="ZH-CN_TOPIC_0000001632419597"></a>

本文中列出的安全加固措施为基本的加固建议项。用户应根据自身业务，重新审视整个系统的网络安全加固措施，必要时可参考业界优秀加固方案和安全专家的建议。


## 操作系统安全加固<a name="ZH-CN_TOPIC_0000001632818973"></a>

### 防火墙配置<a name="ZH-CN_TOPIC_0000001582659054"></a>

操作系统安装后，若配置普通用户，可以通过在“/etc/login.defs”文件中新增“ALWAYS\_SET\_PATH=yes”配置，防止越权操作。


### 设置umask<a name="ZH-CN_TOPIC_0000001582818994"></a>

建议用户将宿主机和容器中的umask设置为027及其以上，提高文件权限。

以设置umask为027为例，具体操作如下所示。

1.  以root用户登录服务器，编辑“/etc/profile”文件。

    ```
    vim /etc/profile
    ```

2.  在“/etc/profile”文件末尾加上**umask 027**，保存并退出。
3.  执行如下命令使配置生效。

    ```
    source /etc/profile
    ```


### 无属主文件安全加固<a name="ZH-CN_TOPIC_0000001582499654"></a>

因为官方Docker镜像与物理机上的操作系统存在差异，系统中的用户可能不能一一对应，导致物理机或容器运行过程中产生的文件变成无属主文件。

用户可以执行**find / -nouser -o -nogroup**命令，查找容器内或物理机上的无属主文件。根据文件的uid和gid创建相应的用户和用户组，或者修改已有用户的uid、用户组的gid来适配，赋予文件属主，避免无属主文件给系统带来安全隐患。


### 端口扫描<a name="ZH-CN_TOPIC_0000001632659605"></a>

需要关注全网侦听的端口和非必要的端口，如有非必要的端口请及时关闭。


### 防DoS攻击<a name="ZH-CN_TOPIC_0000001632579045"></a>

可以通过添加白名单和调整服务组件并发参数大小等方式，防止资源被恶意请求占满。客户端维持连接的时间取决于服务器所设置的“keepAlive”相关参数，请根据实际业务合理设置TCP保活时间、探测次数和探测间隔。


### StreamServer Nginx 安全加固<a name="ZH-CN_TOPIC_0000001724231261"></a>

StreamServer推理服务并不是一个完备的系统，需要通过组件集成方式与用户其他系统配合才能形成一个完整的推理服务系统，建议用户在StreamServer服务前部署Nginx，建议不要使用root用户启动Nginx。同时建议开启Nginx的日志功能，以此记录正常的访问日志和错误请求日志。同时为了防止日志文件过大，需要定时对日志文件进行切割压缩。如果切割压缩后文件仍然过大，可以将切割压缩过的日志文件转储到其它地方。

1.  下载Nginx，例如Ubuntu操作系统执行下方命令，或者使用源码进行安装。安装完成后，需要确保Nginx目录和文件为启动用户修改（权限不高于550）。安装完成后，确保Nginx日志为启动账户修改（权限640），确保Nginx process ID（PID）文件为启动用户修改（权限640）。

    ```bash
    apt install nginx
    ```

2.  设置Nginx配置文件，配置文件要求权限不高于440。

    ```bash
    worker_processes 1;
    worker_cpu_affinity 0001;
    
    worker_rlimit_nofile 4096;
    events {
        worker_connections 4096;
    }
    
    http {
     port_in_redirect off;
     server_tokens off;
     autoindex off;
     
     log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                          '$status $body_bytes_sent "$http_referer" '
                          '"$http_user_agent" "$http_x_forwarded_for" "$request_time"';
           
     access_log /var/log/nginx/access.log main;
     error_log /var/log/nginx/error.log info;
    
     limit_req_zone global zone=req_zone:100m rate=20r/s;
     limit_conn_zone global zone=north_conn_zone:100m;
    
     server {
      listen 127.0.0.1:8081 ssl; # 反向代理的服务端ip及端口，必须配置为服务器ip，不建议设置为空
      server_name localhost;
      
      add_header Referrer-Policy "no-referrer";
      add_header X-XSS-Protection "1; mode=block";
      add_header X-Frame-Options DENY;
      add_header X-Content-Type-Options nosniff;
      add_header Strict-Transport-Security " max-age=31536000; includeSubDomains ";
      add_header Content-Security-Policy "default-src 'self'";
      add_header Cache-control "no-cache, no-store, must-revalidate";
      add_header Pragma no-cache;
      add_header Expires 0;
    
      ssl_session_tickets off;
    
      ssl_certificate     ${path_of_server_crt_1}; # 服务端证书路径(权限400)
      ssl_certificate_key ${path_of_server_key_1}; # 服务端私钥路径，私钥不能明文配置(权限400)
      ssl_client_certificate ${path_of_ca_crt_1}; # 根ca证书路径(权限400)
     
      send_timeout 60;
    
      limit_req zone=req_zone burst=20 nodelay;
      limit_conn north_conn_zone 20;
      keepalive_timeout  60;
      proxy_read_timeout 900;
      proxy_connect_timeout   60;
      proxy_send_timeout      60;
      client_header_timeout   60;
      client_body_timeout 10;
      client_header_buffer_size  2k;
      large_client_header_buffers 4 8k;
      client_body_buffer_size 16K;
      client_max_body_size 20m;
      ssl_protocols TLSv1.2 TLSv1.3;
      ssl_ciphers "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS !RC4"; 
      
      ssl_verify_client on;
      ssl_verify_depth 9; 
      ssl_session_timeout 10s;
      ssl_session_cache shared:SSL:10m;
    
      location / {
       limit_except OPTIONS HEAD {
        deny all;
       }
       proxy_hide_header X-Powered-By;
      }
    
      location /v2 {
       proxy_pass https://127.0.0.1:8080; # 需要设置为StreamServer配置文件配置的ip及端口
       allow 127.0.0.1; #需要设置允许访问的远端ip
       deny all;
       proxy_ssl_certificate     ${path_of_server_crt_2}; # 服务端证书路径，建议配置通过certImport.sh脚本导入到${MX_SDK_HOME}/samples/mxVision/streamserver/keys目录下的server.crt (权限400)
       proxy_ssl_certificate_key ${path_of_server_key_2}; # 服务端私钥路径，私钥不能明文配置，建议配置通过certImport.sh脚本导入到${MX_SDK_HOME}/samples/mxVision/streamserver/keys目录下的server.key (权限400)
       proxy_ssl_trusted_certificate ${path_of_ca_crt_2}; # 根ca证书路径，建议配置通过certImport.sh脚本导入到${MX_SDK_HOME}/samples/mxVision/streamserver/keys目录下的ca.crt (权限400)
       proxy_ssl_session_reuse on;
       proxy_ssl_protocols TLSv1.2 TLSv1.3;
       proxy_ssl_ciphers "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384";
      }
     }
    }
    ```

3.  启动Nginx，使用<b>-c</b>命令传入配置文件路径。$\{path\_of\_nginx\_bin\}为已安装的Nginx的二进制路径，不同环境或者安装方式生成的路径可能不同。

    ```bash
    ${path_of_nginx_bin} -c ${path_of_nginx_config_file} # Nginx配置文件
    ```



