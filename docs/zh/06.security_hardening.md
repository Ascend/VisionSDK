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

1. 以root用户登录服务器，编辑“/etc/profile”文件。

    ```bash
    vim /etc/profile
    ```

2. 在“/etc/profile”文件末尾加上**umask 027**，保存并退出。
3. 执行如下命令使配置生效。

    ```bash
    source /etc/profile
    ```

### 无属主文件安全加固<a name="ZH-CN_TOPIC_0000001582499654"></a>

因为官方Docker镜像与物理机上的操作系统存在差异，系统中的用户可能不能一一对应，导致物理机或容器运行过程中产生的文件变成无属主文件。

用户可以执行**find / -nouser -o -nogroup**命令，查找容器内或物理机上的无属主文件。根据文件的uid和gid创建相应的用户和用户组，或者修改已有用户的uid、用户组的gid来适配，赋予文件属主，避免无属主文件给系统带来安全隐患。

### 端口扫描<a name="ZH-CN_TOPIC_0000001632659605"></a>

需要关注全网侦听的端口和非必要的端口，如有非必要的端口请及时关闭。

### 防DoS攻击<a name="ZH-CN_TOPIC_0000001632579045"></a>

可以通过添加白名单和调整服务组件并发参数大小等方式，防止资源被恶意请求占满。客户端维持连接的时间取决于服务器所设置的“keepalive”相关参数，请根据实际业务合理设置TCP保活时间、探测次数和探测间隔。

## StreamServer安全加固<a name="ZH-CN_TOPIC_0000001572082912"></a>

### Nginx网关<a name="ZH-CN_TOPIC_0000002463250302"></a>

StreamServer监听至127.0.0.1且StreamServer和公网、局域网隔离需由用户保证。StreamServer需要通过组件集成方式与用户的其他系统配合才能形成一个完整的推理服务系统，如可使用开源软件Nginx进行保障，用户可参照Nginx官方文档进行Nginx的部署。建议专门创建Nginx运行用户而不要使用root用户启动Nginx。建议开启Nginx的日志功能，用于记录正常的访问日志和错误请求日志。为了防止日志文件过大，需要定时对日志文件进行切割压缩。如果切割压缩后文件仍然过大，可以将切割压缩过的日志文件转储到其它地方。

> [!NOTE]
>
>- 请及时更新Nginx补丁或升级版本，使用最新最稳定的安全版本。
>- 建议Web应用目录及文件属主只能为root或Nginx运行用户，且只允许属主可读写执行。
>- 建议Nginx根目录只能由root或Nginx运行用户修改，Nginx根目录的所有父级目录的修改权限不能赋予除root和Nginx运行用户的其他普通用户。
>- 建议Nginx日志文件属主只能为Nginx运行用户，且只允许属主可读写。

**操作步骤<a name="section1363491315542"></a>**

1. 安装Nginx。可以使用源码进行安装，也可以使用包管理安装。如在Ubuntu操作系统下可执行下面命令进行安装。安装完成后，需要确保Nginx目录和文件为启动用户修改（权限不高于550）并确保Nginx日志为启动账户修改（权限640），确保Nginx process ID（PID）文件为启动用户修改（权限640）。

    ```bash
    apt install nginx
    ```

    > [!NOTE]
    >建议使用非root用户启动Nginx。如果要使用root用户启动Nginx，则需要创建专门执行Nginx的用户，可使用如下命令创建：
    >①**创建用户**
    > 使用以下命令创建一个名为nginx-user的用户，该用户没有登录权限
    >
    > ```bash
    > sudo useradd -r -s /sbin/nologin nginx-user
    > ```
    >
    >②**修改Nginx配置文件**
    > 在Nginx配置文件（通常是/etc/nginx/nginx.conf）的最顶部，添加或修改user指令，如下所示
    >
    > ```bash
    > user nginx-user;
    > ```
    >
    >③**设置文件和目录权限**
    > 确保Nginx需要访问的文件和目录对nginx-user用户可读可写。例如，对于日志目录和缓存目录，可使用以下命令设置权限
    >
    > ```bash
    > sudo chown -R nginx-user:nginx-user /path/to/log/nginx
    > ```

2. 设置Nginx配置文件，配置文件要求权限不高于440。Nginx参考配置如下：

    - 访问控制加固

        IP白名单配置

        ```nginx
            # 限制只有特定IP可以访问API(示例IP)
            location / {
                allow 192.168.1.0/24;
                allow 10.0.0.0/8;
                deny all;
            }
        ```

    - 速率限制防护

        请求频率限制

        ```nginx
            # 定义限制策略
            limit_req_zone global zone=req_zone:100m rate=60r/m;

            server {
                # 应用速率限制
                location / {
                    limit_req zone=api burst=20 nodelay;
                    limit_req_status 429;
                }
            }
        ```

        并发连接限制

        ```nginx
            # 限制单个IP的并发连接数
            limit_conn_zone $binary_remote_addr zone=conn_limit_per_ip:10m;

            server {
                limit_conn conn_limit_per_ip 10;
            }
        ```

    - SSL/TLS安全加固

        强化SSL配置

        ```nginx
            server {
                listen 10.0.0.0:8001 ssl;

                # 使用强加密套件
                ssl_protocols TLSv1.2 TLSv1.3;
                ssl_ciphers "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS !RC4";
                ssl_prefer_server_ciphers on;

                # 启用HSTS
                add_header Strict-Transport-Security "max-age=31536000; includeSubDomains; preload";

                # 防止协议降级
                ssl_stapling on;
                ssl_stapling_verify on;
            }
        ```

    - 请求内容安全控制

        请求大小限制

        ```nginx
            server {
                # 限制客户端请求体大小
                client_max_body_size 50M;

                # 限制请求头大小
                large_client_header_buffers 200 8k;
            }
        ```

        超时控制

        ```nginx
            server {
                # 设置各种超时时间防止资源耗尽
                client_body_timeout 120s;
                client_header_timeout 120s;
                keepalive_timeout 65s;
                keepalive_requests 200;
                send_timeout 120s;
            }
        ```

    - 安全头设置

        添加安全相关HTTP头

        ```nginx
            server {
                # 防止点击劫持
                add_header X-Frame-Options "SAMEORIGIN" always;

                # 防止XSS攻击
                add_header X-XSS-Protection "1; mode=block" always;

                # 禁止 MIME 类型嗅探
                add_header X-Content-Type-Options "nosniff" always;

                # 控制referrer信息
                add_header Referrer-Policy "no-referrer-when-downgrade" always;

                # 内容安全策略
                add_header Content-Security-Policy "default-src 'self' http: https: data: blob: 'unsafe-inline'" always;
            }
        ```

    - 日志和监控

        详细日志记录

        ```nginx
            # 自定义日志格式记录安全相关信息
            log_format security_log '$remote_addr - $remote_user [$time_local] "$request" '
                                   '$status $body_bytes_sent "$http_referer" '
                                   '"$http_user_agent" "$http_x_forwarded_for" '
                                   '$request_time $upstream_response_time';

            access_log $HOME/log/nginx/security.log security_log;
            error_log $HOME/log/nginx/error.log warn;
        ```

        异常请求监控

        ```nginx
            # 记录可疑请求
            location ~* (\.php|\.asp|\.exe|\.sh|\.bash) {
                access_log $HOME/log/nginx/suspicious.log;
                return 403;
            }
        ```

    - 隐藏服务器信息

        ```nginx
            server {
                # 隐藏nginx版本信息
                server_tokens off;

                # 自定义错误页面
                error_page 403 /error/403.html;
                error_page 404 /error/404.html;
                error_page 500 502 503 504 /error/50x.html;
            }
        ```

    - 上游服务保护

        ```nginx
            location / {
                # 请以实际StreamServer监听端口为准
                proxy_pass http://127.0.0.1:8000;

                # 隐藏真实客户端IP
                proxy_hide_header X-Powered-By;

                # Nginx 先完整读请求体
                proxy_request_buffering on;

                # 设置代理超时
                proxy_connect_timeout 30s;
                proxy_send_timeout 30s;
                proxy_read_timeout 30s;

                # 限制后端重试次数
                proxy_next_upstream error timeout invalid_header http_500 http_502 http_503;
                proxy_next_upstream_tries 2;

                # 指定 X-Real-IP 和 X-Forwarded-For
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
                proxy_set_header X-Forwarded-For $remote_addr;
                proxy_set_header X-Forwarded-Proto $scheme;
            }
        ```

    - 降低CSRF攻击风险

        ```nginx
            server {
                add_header Strict-Transport-Security "max-age=31536000; includeSubDomains; preload";
                add_header X-Frame-Options "DENY";
                add_header Content-Security-Policy "default-src 'self'; frame-ancestors 'none';";
                proxy_read_timeout 900;
                proxy_connect_timeout 60;
                proxy_send_timeout 60;
                limit_req zone=req_zone burst=20 nodelay;
                limit_conn north_conn_zone 512;
            }
            location / {
              limit_except GET POST HEAD {
                 deny all;
              add_header X-Powered-By '';
              allow 10.0.0.0; # 允许的IP地址
              deny all;       # 拒绝其他所有IP
            }
        ```

    - 增强SSL/TLS连接的安全性

        使用至少2048位的Diffie-Hellman（DHE）参数，可通过生成一个DHE参数文件并在Nginx配置中引用它来实现。

        - **生成DHE参数文件**

            使用OpenSSL生成一个2048位的DHE参数文件

            ```bash
            openssl dhparam -out /path/to/dhparam.pem 2048
            ```

        - **在Nginx配置中引用DHE参数文件**

            在Nginx配置文件中，添加ssl\_dhparam指令来引用生成的DHE参数文件。例如：

            ```nginx
            ssl_dhparam /path/to/dhparam.pem;
            ```

    **Nginx配置文件示例**

    设置Nginx配置文件，配置文件要求权限不高于440，默认路径为: /etc/nginx/nginx.conf，access\_log与error\_log需配置相应路径。

    ```nginx
    # 如果以root用户启动Nginx，则需要指定工作用户；如果以非root用户启动Nginx，则无需配置工作用户
    user nginx-user;

    worker_processes 1;
    worker_cpu_affinity 0001;
    worker_rlimit_nofile 4096;
    events {
        worker_connections 512;
    }
    http {
    port_in_redirect off;
    server_tokens off;
    autoindex off;
    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                          '$status $body_bytes_sent "$http_referer" '
                          '"$http_user_agent" "$http_x_forwarded_for" "$request_time"';

    access_log /path/log/nginx/access.log main;
    error_log /path/log/nginx/error.log info;
    limit_req_zone global zone=req_zone:100m rate=60r/m;
    limit_conn_zone global zone=north_conn_zone:100m;
    # HTTPS请求
      server {
      listen 10.0.0.0:8001 ssl; # 反向代理的服务端ip及端口（示例），必须配置为真实远端ip，不建议设置为空
      server_name localhost;

      add_header Referrer-Policy "no-referrer";
      add_header X-XSS-Protection "1; mode=block";
      add_header X-Frame-Options DENY;
      add_header X-Content-Type-Options nosniff;
      add_header Strict-Transport-Security "max-age=31536000; includeSubDomains; preload";
      add_header Content-Security-Policy "default-src 'self'; frame-ancestors 'none';";
      add_header Cache-control "no-cache, no-store, must-revalidate";
      add_header Pragma no-cache;
      add_header Expires 0;
      ssl_stapling on;
      ssl_stapling_verify on;
      ssl_session_tickets off;
      ssl_certificate     ${path_of_server_crt_1}; # 服务端证书路径，需要用户自行配置(权限400)
      ssl_certificate_key ${path_of_server_key_1}; # 服务端私钥路径，需要用户自行配置，私钥不能明文配置(权限400)
      ssl_client_certificate ${path_of_ca_crt_1}; # 根ca证书路径，需要用户自行配置(权限400)
      send_timeout 60;
      limit_req zone=req_zone burst=20 nodelay;
      limit_conn north_conn_zone 512;
      keepalive_timeout  60;
      proxy_read_timeout 900;
      proxy_connect_timeout   60;
      proxy_send_timeout      60;
      client_header_timeout   60;
      client_body_timeout 10;
      client_header_buffer_size  8k; # 高并发与长上下文场景按需增加
      large_client_header_buffers 200 8k; # 高并发与长上下文场景按需增加
      client_body_buffer_size 50m; # 高并发与长上下文场景按需增加
      client_max_body_size 50m; # 高并发与长上下文场景按需增加
      ssl_protocols TLSv1.2 TLSv1.3;
      ssl_ciphers "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS !RC4";
      ssl_dhparam /path/to/dhparam.pem; # 配置为实际DHE参数文件路径

      ssl_verify_client on;
      ssl_verify_depth 9;
      ssl_session_timeout 10s;
      ssl_session_cache shared:SSL:10m;
      location / {
        limit_except GET POST HEAD {
           deny all;
        }
        dav_methods off;
        add_header X-Powered-By '';
        proxy_pass http://127.0.0.1:8000; # 需要设置为StreamServer提供推理服务的ip及端口
        allow 10.0.0.0; #需要设置允许访问的远端实际ip
        deny all;
        proxy_set_header Host 127.0.0.1;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $remote_addr;
        proxy_set_header X-Forwarded-Proto $scheme;
      }
      }
    }
    ```

3. 若要定制Nginx的出错信息，需要使用error\_page指令来定义特定HTTP状态码的自定义错误页面。添加自定义错误页面步骤如下：
    1. **创建自定义错误页面文件**：

        首先，创建HTML文件，这些文件将作为自定义错误页面。例如，创建一个名为404.html的文件用于404错误，一个名为500.html的文件用于500错误等。这些文件可以包含自定义HTML内容。

    2. **配置error\_page指令**

        在Nginx配置文件中，使用error\_page指令来关联HTTP状态码和对应的自定义错误页面。例如：

        ```nginx
        error_page 404 /404.html;
        error_page 500 502 503 504 /50x.html;
        ```

    3. **确保错误页面可访问**

        确保Nginx能够访问到这些自定义错误页面文件。通常，这些文件应该放在Nginx的html目录下，或者其他正确位置。

    4. **完整配置示例**

        ```nginx
        http {
            # ... 其他配置 ...
            # 定义自定义错误页面
            error_page 404 /404.html;
            error_page 500 502 503 504 /50x.html;
            server {
                 listen 10.0.0.0:8001 ssl;
                 server_name localhost;
                 # ... 其他配置 ...
                 # 确保Nginx知道在哪里找到错误页面
                 root /path/to/your/html;
                 # ... 其他配置 ...
            }
         }
        ```

4. 启动Nginx，使用**-c**命令传入配置文件路径。$\{path\_of\_nginx\_bin\}为已安装的Nginx的二进制路径，不同环境或者安装方式生成的路径可能不同。

    ```bash
    ${path_of_nginx_bin} -c ${path_of_nginx_config_file} # Nginx配置文件
    ```
