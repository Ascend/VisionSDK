# Security Hardening

## Security Requirements

When you use an API to read a file, ensure that you own the file and that its permissions are no more permissive than `640` to prevent privilege escalation and similar security issues.

Software code or programs downloaded from external sources may pose risks. You must ensure that the related functions are secure.

## Hardening Precautions

The security hardening measures listed in this document are basic recommendations. You should re-evaluate the network security hardening measures for the entire system based on your own business needs. When necessary, you can refer to industry best practices and the advice of security experts.

## OS Security Hardening

### Firewall Configuration

After the operating system is installed, if common users are configured, you can add `ALWAYS_SET_PATH=yes` to the `/etc/login.defs` file to prevent unauthorized operations.

### Setting umask

You are advised to set the umask on the host and in containers to `027` or a more restrictive value to tighten file permissions.

To set `umask` to `027`:

1. Log in to the server as the root user and edit the `/etc/profile` file.

    ```bash
    vim /etc/profile
    ```

2. Add `umask 027` to the end of the `/etc/profile` file, then save and exit.
3. Run the following command to apply the configuration.

    ```bash
    source /etc/profile
    ```

### Ownerless File Hardening

Because official Docker images differ from the operating system on the physical machine, system users may not correspond one to one. Therefore, files created on the physical machine or in containers can become ownerless.

You can run `find / -nouser -o -nogroup` to locate ownerless files in a container or on the physical machine. Create corresponding users and groups based on the file UID and GID, or adjust existing user UIDs and group GIDs to match them. Then assign ownership to the files and prevent ownerless files from creating security risks for the system.

### Port Scanning

Monitor ports that listen on all interfaces and any unnecessary ports. If you find unnecessary ports, close them promptly.

### Anti-DoS Protection

You can protect the system against DoS attacks by adding allowlists and tuning the concurrency settings of service components. The time a client keeps a connection open depends on the `keepalive`-related parameters of the server. Set TCP keepalive time, probe count, and probe interval based on actual service needs.

## StreamServer Security Hardening

### Nginx Gateway

StreamServer listens on 127.0.0.1, and you must ensure that StreamServer is isolated from the public Internet and the local area network. StreamServer needs to integrate with other user systems through components to form a complete inference service system. You can use open-source software Nginx for protection, and you can refer to the official Nginx documentation for deployment. You are advised to create a dedicated Nginx runtime user instead of using the root user to start Nginx. You are advised to enable Nginx logging to record normal access logs and error request logs. To prevent log files from growing too large, regularly split and compress them. If the files are still too large after splitting and compression, you can archive the split and compressed log files elsewhere.

> [!NOTE]
>
>- Update Nginx patches or upgrade the version in time, and use the latest and most stable secure version.
>- Ensure that the owner of Web application directories and files is either root or the Nginx runtime user, and that only the owner has read, write, and execute permissions.
>- Ensure that only root or the Nginx runtime user can modify the Nginx root directory, and that you do not grant write permissions on any parent directory of the Nginx root directory to other common users besides root and the Nginx runtime user.
>- Ensure that the owner of Nginx log files is only the Nginx runtime user, and that only the owner has read and write permissions.

**Operation Steps**

1. Install Nginx. You can install it from source or with a package manager. For example, on Ubuntu, you can run the following command to install it. After installation, ensure that the Nginx directories and files are writable by the startup user, with permissions no more permissive than 550. Ensure that the Nginx logs are writable by the startup account, with permissions 640, and ensure that the Nginx process ID (PID) file is writable by the startup user, with permissions 640.

    ```bash
    apt install nginx
    ```

    After installation, ensure that the Nginx directories and files are writable by the startup user, with permissions no more permissive than 550. Ensure that the Nginx logs are writable by the startup account, with permissions 640, and ensure that the Nginx process ID (PID) file is writable by the startup user, with permissions 640.

    > [!NOTE]
    > You are advised to start Nginx as a non-root user. If you must start Nginx as the root user, you need to create a dedicated user for running Nginx. You can run the following commands to create it:
    > 1. **Create the user**
    >    Create a user named `nginx-user` with no login permissions by running the following command.
    >
    >    ```bash
    >    sudo useradd -r -s /sbin/nologin nginx-user
    >    ```
    >
    > 2. **Modify the Nginx configuration file**
    >    At the top of the Nginx configuration file, which is usually `/etc/nginx/nginx.conf`, add or modify the `user` directive as follows.
    >
    >    ```bash
    >    user nginx-user;
    >    ```
    >
    > 3. **Set file and directory permissions**
    >    Ensure that the files and directories that Nginx needs to access are readable and writable by the `nginx-user` user. For example, you can run the following command to set permissions for the log directory and cache directory.
    >
    >    ```bash
    >    sudo chown -R nginx-user:nginx-user /path/to/log/nginx
    >    ```

2. Set the Nginx configuration file. The file permissions must not be higher than 440. The following is a reference Nginx configuration:

    - Access control hardening

        IP address allowlist configuration

        ```nginx
            # Limit API access to specific IP addresses only (example IP addresses)
            location / {
                allow 192.168.1.0/24;
                allow 10.0.0.0/8;
                deny all;
            }
        ```

    - Rate limiting protection

        Request rate limiting

        ```nginx
            # Define the limiting policy
            limit_req_zone global zone=req_zone:100m rate=60r/m;

            server {
                # Apply rate limiting
                location / {
                    limit_req zone=api burst=20 nodelay;
                    limit_req_status 429;
                }
            }
        ```

        Concurrent connection limits

        ```nginx
            # Limit the number of concurrent connections for a single IP address
            limit_conn_zone $binary_remote_addr zone=conn_limit_per_ip:10m;

            server {
                limit_conn conn_limit_per_ip 10;
            }
        ```

    - SSL/TLS security hardening

        Harden the SSL configuration.

        ```nginx
            server {
                listen 10.0.0.0:8001 ssl;

                # Use strong cipher suites
                ssl_protocols TLSv1.2 TLSv1.3;
                ssl_ciphers "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS !RC4";
                ssl_prefer_server_ciphers on;

                # Enable HSTS
                add_header Strict-Transport-Security "max-age=31536000; includeSubDomains; preload";

                # Prevent protocol downgrade attacks
                ssl_stapling on;
                ssl_stapling_verify on;
            }
        ```

    - Request content security control

        Request size limits

        ```nginx
            server {
                # Limit the client request body size
                client_max_body_size 50M;

                # Limit the request header size
                large_client_header_buffers 200 8k;
            }
        ```

        Timeout control

        ```nginx
            server {
                # Set various timeout values to prevent resource exhaustion
                client_body_timeout 120s;
                client_header_timeout 120s;
                keepalive_timeout 65s;
                keepalive_requests 200;
                send_timeout 120s;
            }
        ```

    - Security header settings

        Add security-related HTTP headers.

        ```nginx
            server {
                # Prevent clickjacking
                add_header X-Frame-Options "SAMEORIGIN" always;

                # Prevent XSS attacks
                add_header X-XSS-Protection "1; mode=block" always;

                # Disable MIME type sniffing
                add_header X-Content-Type-Options "nosniff" always;

                # Control referrer information
                add_header Referrer-Policy "no-referrer-when-downgrade" always;

                # Content Security Policy
                add_header Content-Security-Policy "default-src 'self' http: https: data: blob: 'unsafe-inline'" always;
            }
        ```

    - Logs and monitoring

        Detailed logging

        ```nginx
            # Customize the log format to record security-related information
            log_format security_log '$remote_addr - $remote_user [$time_local] "$request" '
                                   '$status $body_bytes_sent "$http_referer" '
                                   '"$http_user_agent" "$http_x_forwarded_for" '
                                   '$request_time $upstream_response_time';

            access_log $HOME/log/nginx/security.log security_log;
            error_log $HOME/log/nginx/error.log warn;
        ```

        Abnormal request monitoring

        ```nginx
            # Record suspicious requests
            location ~* (\.php|\.asp|\.exe|\.sh|\.bash) {
                access_log $HOME/log/nginx/suspicious.log;
                return 403;
            }
        ```

    - Hide server information.

        ```nginx
            server {
                # Hide Nginx version information
                server_tokens off;

                # Custom error pages
                error_page 403 /error/403.html;
                error_page 404 /error/404.html;
                error_page 500 502 503 504 /error/50x.html;
            }
        ```

    - Upstream service protection

        ```nginx
            location / {
                # Use the actual StreamServer listening port
                proxy_pass http://127.0.0.1:8000;

                # Hide the real client IP address
                proxy_hide_header X-Powered-By;

                # Let Nginx read the entire request body first
                proxy_request_buffering on;

                # Set proxy timeouts
                proxy_connect_timeout 30s;
                proxy_send_timeout 30s;
                proxy_read_timeout 30s;

                # Limit backend retry attempts
                proxy_next_upstream error timeout invalid_header http_500 http_502 http_503;
                proxy_next_upstream_tries 2;

                # Set X-Real-IP and X-Forwarded-For
                proxy_set_header Host $host;
                proxy_set_header X-Real-IP $remote_addr;
                proxy_set_header X-Forwarded-For $remote_addr;
                proxy_set_header X-Forwarded-Proto $scheme;
            }
        ```

    - Reduce the risk of CSRF attacks.

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
              allow 10.0.0.0; # Allowed IP address
              deny all;       # Deny all other IP addresses
            }
        ```

    - Improve the security of SSL/TLS connections.

        Use Diffie-Hellman (DHE) parameters with at least 2048 bits. You can do this by generating a DHE parameter file and referencing it in the Nginx configuration.

        - **Generate the DHE parameter file**

            Use OpenSSL to generate a 2048-bit DHE parameter file.

            ```bash
            openssl dhparam -out /path/to/dhparam.pem 2048
            ```

        - **Reference the DHE parameter file in the Nginx configuration**

            In the Nginx configuration file, add the `ssl_dhparam` directive to reference the generated DHE parameter file. For example:

            ```nginx
            ssl_dhparam /path/to/dhparam.pem;
            ```

    **Nginx Configuration Example**

    Set the Nginx configuration file. The file permissions must not be higher than 440. The default path is `/etc/nginx/nginx.conf`, and configure the corresponding paths for `access_log` and `error_log`.

    ```nginx
    # If you start Nginx as the root user, you must specify a working user. If you start Nginx as a non-root user, you do not need to configure a working user
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
    # HTTPS requests
      server {
      listen 10.0.0.0:8001 ssl; # Backend service IP address and port for the reverse proxy (example). You must configure the real remote IP address. Do not leave it empty
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
      ssl_certificate     ${path_of_server_crt_1}; # Server certificate path. You need to configure it yourself (permissions 400)
      ssl_certificate_key ${path_of_server_key_1}; # Server private key path. You need to configure it yourself. Do not configure the private key in plaintext (permissions 400)
      ssl_client_certificate ${path_of_ca_crt_1}; # Root CA certificate path. You need to configure it yourself (permissions 400)
      send_timeout 60;
      limit_req zone=req_zone burst=20 nodelay;
      limit_conn north_conn_zone 512;
      keepalive_timeout  60;
      proxy_read_timeout 900;
      proxy_connect_timeout   60;
      proxy_send_timeout      60;
      client_header_timeout   60;
      client_body_timeout 10;
      client_header_buffer_size  8k; # Increase as needed for high-concurrency and long-context scenarios
      large_client_header_buffers 200 8k; # Increase as needed for high-concurrency and long-context scenarios
      client_body_buffer_size 50m; # Increase as needed for high-concurrency and long-context scenarios
      client_max_body_size 50m; # Increase as needed for high-concurrency and long-context scenarios
      ssl_protocols TLSv1.2 TLSv1.3;
      ssl_ciphers "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256 !aNULL !eNULL !LOW !3DES !MD5 !EXP !PSK !SRP !DSS !RC4";
      ssl_dhparam /path/to/dhparam.pem; # Set this to the actual DHE parameter file path

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
        proxy_pass http://127.0.0.1:8000; # Set this to the IP address and port that StreamServer uses to provide inference services
        allow 10.0.0.0; # Set the actual remote IP address that is allowed access
        deny all;
        proxy_set_header Host 127.0.0.1;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $remote_addr;
        proxy_set_header X-Forwarded-Proto $scheme;
      }
      }
    }
    ```

3. To customize Nginx error messages, use the `error_page` directive to define custom error pages for specific HTTP status codes. Follow these steps to add custom error pages:
    1. **Create the custom error page files.**

        First, create HTML files that serve as custom error pages. For example, create a file named `404.html` for `404` errors, a file named `500.html` for `500` errors, and so on. These files can contain custom HTML content.

    2. **Configure the `error_page` directive.**

        In the Nginx configuration file, use the `error_page` directive to associate HTTP status codes with the corresponding custom error pages. For example:

        ```nginx
        error_page 404 /404.html;
        error_page 500 502 503 504 /50x.html;
        ```

    3. **Ensure that the error pages are accessible.**

        Ensure that Nginx can access these custom error page files. Typically, you should place these files in the Nginx `html` directory or another correct location.

    4. **Complete configuration example.**

        ```nginx
        http {
            # ... Other configuration ...
            # Define custom error pages
            error_page 404 /404.html;
            error_page 500 502 503 504 /50x.html;
            server {
                 listen 10.0.0.0:8001 ssl;
                 server_name localhost;
                 # ... Other configuration ...
                 # Ensure that Nginx knows where to find the error pages
                 root /path/to/your/html;
                 # ... Other configuration ...
            }
         }
        ```

4. Start Nginx and pass the configuration file path with the `-c` option. `${path_of_nginx_bin}` is the path to the installed Nginx binary, and the path may differ depending on the environment and installation method.

    ```bash
    ${path_of_nginx_bin} -c ${path_of_nginx_config_file} # Nginx configuration file
    ```
