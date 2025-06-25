

# Running the Server

This guide explains how to run the Node.js server and manage it using PM2 for better reliability and logging.

## Basic Start

To start the server, simply run:

```bash
node server.js
```

If any required Node.js modules are missing, please install them individually using `npm install <module_name>`.

## Recommended: Use PM2 for Process Management

We recommend using PM2 to manage the server process. PM2 helps keep your server running continuously and provides built-in log management.

Start the server with PM2 using the following command:

```
pm2 start server.js --name my-server --output ~/logs/server/out.log --error ~/logs/server/error.log
```

This will:

- Name the process `my-server`
- Save standard output logs to `~/logs/server/out.log`
- Save error logs to `~/logs/server/error.log`

Below is an example of the server running under PM2:

![Server running with PM2](https://iammeterglobal.oss-accelerate.aliyuncs.com/img/image-20250625123055054.png)



# demo

http://laoliu.iammeter.com:3000/

[About WattBattle](https://www.iammeter.com/blog/WattBattle-202506)