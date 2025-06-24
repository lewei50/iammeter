This project primarily consists of demo codes developed around the [IAMMETER meter API](https://www.iammeter.com/newsshow/blog-fw-features), the [WPC3700 Power Adjustment Device API](https://www.iammeter.com/newsshow/api-wpc3700), and the [IAMMETER-Cloud API](https://www.iammeter.com/docs/system-api), aiming to help users better understand how to use these APIs.

# cloudapi-1-Node.js

A Node.js script that calls the [IAMMETER-Cloud API](https://www.iammeter.com/docs/system-api) to analyze the per-phase generation data of a meter for the current day, based on the meter's SN.

# Tools

In addition to using the IAMMETER-Cloud service, IAMMETER meters can also be configured to upload data to the user's own server using different protocols (TCP, TLS, HTTP, HTTPS, MQTT).

![Settings Page](https://iammeterglobal.oss-accelerate.aliyuncs.com/img/image-20241108151423288.png)

This directory provides some simple examples of TCP servers, TLS servers, and HTTP servers to help users test the functionality of configuring meters to upload data to third-party servers.
