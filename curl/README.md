# Djazair Curl Extension Manual

The **cURL** extension is the official, high-performance module for executing network requests and HTTP/HTTPS communications in the Djazair programming language. Powered by a native binding to the robust **libcurl** library, it supports secure SSL protocols, cookie management, proxies, custom HTTP methods, and detailed transfer info.

---

## 📋 Table of Contents
1. [Installation & Setup](#1-installation--setup)
2. [Importing & Quick Start](#2-importing--quick-start)
3. [Class Reference](#3-class-reference)
4. [Constants Reference](#4-constants-reference)
5. [SSL & Windows Certificate Caveats](#5-ssl--windows-certificate-caveats)
6. [Advanced Recipes](#6-advanced-recipes)
    - [Downloading a File to Disk](#a-downloading-a-file-to-disk)
    - [Sending a POST Request with JSON Payload](#b-sending-a-post-request-with-json-payload)
    - [Managing Cookies](#c-managing-cookies)
    - [Connecting Through a Proxy](#d-connecting-through-a-proxy)

---

## 1. Installation & Setup

### Install via DPM (Package Manager)
To add the curl extension to your project, run the following DPM command:
```bash
dpm install curl
```

### Building the Native Module (DLL compilation)
Because the module relies on native C code (`src/curl.c` and `src/curl_native.c`), it must be compiled into a dynamic library (`curl.dll`):
```bash
dpm build curl
```
**Compilation Prerequisites:**
1. A C compiler (such as `gcc` or `clang` / MinGW) must be installed and added to your system environment `PATH`.
2. The `libcurl` library must be installed on your operating system (on Windows, MSYS2/MinGW packages are recommended).

---

## 2. Importing & Quick Start

To import the curl module, use the `use` keyword:
```djazair
use curl
```
After importing, you can instantiate the `Curl` class:
```djazair
let client = curl.Curl()
```

---

## 3. Class Reference

### Class `curl.Curl`
The main class responsible for managing and executing HTTP connections.

#### `init()`
*   **Description:** Initializes a new cURL easy handle.
*   **Return Type:** `void`.
*   **Throws:** Throws a `"CurlError"` if `libcurl` is not installed or cannot be loaded.

#### `setopt(option, value)`
*   **Description:** Sets specific easy handle options.
*   **Parameters:**
    *   `option` (Number): The numeric constant of the option to set (see Constants Reference).
    *   `value` (String | Number | Bool): The value to assign to the option.
*   **Return Type:** `Bool` (`True` on success, `False` on failure).
*   **Throws:** Throws `ValueError` if the value is `Null`, or `TypeError` if the type is unsupported.

#### `setHeaders(headersList)`
*   **Description:** Sets custom HTTP headers for the request.
*   **Parameters:**
    *   `headersList` (Array): A list of strings in the format `"Key: Value"`.
*   **Return Type:** `Bool`.
*   **Example:** `client.setHeaders(["Accept: application/json", "Authorization: Bearer my_token"])`

#### `perform()`
*   **Description:** Synchronously executes the network request.
*   **Return Type:** `String` containing the response body, or `Null` if the connection fails.

#### `getInfo(info)`
*   **Description:** Retrieves connection and transfer metadata after execution.
*   **Parameters:**
    *   `info` (Number): The numeric constant of the metadata to retrieve (e.g., `curl.CURL_INFO_RESPONSE_CODE`).
*   **Return Type:** `String` or `Number` depending on the type of info requested.

#### `getResponseHeaders()`
*   **Description:** Returns the HTTP response headers sent by the server.
*   **Return Type:** `Map` (lowercase header names as keys, header values as map values).

#### `getError()`
*   **Description:** Returns the last human-readable error message recorded by cURL if `perform()` fails.
*   **Return Type:** `String`.

#### `cleanup()`
*   **Description:** Closes the easy handle and frees all allocated memory.
*   **Return Type:** `void`.
*   **Note:** Always invoke this method in a `finally` block to prevent memory leaks.

---

## 4. Constants Reference

The extension exposes the following constants for setting options and retrieving transfer metadata:

### Request and Connection Options (`CURL_OPT_*`)
| Constant | Numeric Value | Description |
| :--- | :--- | :--- |
| `curl.CURL_OPT_URL` | `10002` | Sets the target URL. |
| `curl.CURL_OPT_USERAGENT` | `10018` | Sets the request's User-Agent string. |
| `curl.CURL_OPT_TIMEOUT` | `13` | Sets the maximum time in seconds the request can take. |
| `curl.CURL_OPT_CONNECTTIMEOUT` | `78` | Sets the maximum connection timeout in seconds. |
| `curl.CURL_OPT_FOLLOWLOCATION` | `52` | Follow HTTP redirects (3xx) if set to `True`. |
| `curl.CURL_OPT_MAXREDIRS` | `68` | Sets the maximum number of redirects to follow. |
| `curl.CURL_OPT_PROXY` | `10004` | Sets the proxy server URL. |
| `curl.CURL_OPT_USERPWD` | `10005` | Basic authentication string in format `"user:password"`. |

### SSL and Security Options
| Constant | Numeric Value | Description |
| :--- | :--- | :--- |
| `curl.CURL_OPT_SSL_VERIFYPEER` | `64` | Verify the peer's SSL certificate (`True` or `False`). |
| `curl.CURL_OPT_SSL_VERIFYHOST` | `81` | Verify the hostname in the SSL certificate (`2` to enable, `0` to disable). |
| `curl.CURL_OPT_CAINFO` | `10065` | Path to local Certificate Authority (CA) bundle. |

### Cookie and Custom Methods
| Constant | Numeric Value | Description |
| :--- | :--- | :--- |
| `curl.CURL_OPT_POST` | `47` | Sets request method to POST. |
| `curl.CURL_OPT_POSTFIELDS` | `10015` | Payload data attached to a POST request. |
| `curl.CURL_OPT_CUSTOMREQUEST` | `10036` | Sets a custom HTTP method (e.g., `"PUT"`, `"DELETE"`, `"PATCH"`). |
| `curl.CURL_OPT_COOKIE` | `10022` | Send raw Cookie strings in the headers. |
| `curl.CURL_OPT_COOKIEFILE` | `10031` | Read session cookies from a local file. |
| `curl.CURL_OPT_COOKIEJAR` | `10082` | Write new cookies received from the server to a local file. |

### Transfer Information (`CURL_INFO_*`)
| Constant | Numeric Value | Description |
| :--- | :--- | :--- |
| `curl.CURL_INFO_RESPONSE_CODE` | `2097154` | The HTTP status code received (e.g., 200, 404, 500). |
| `curl.CURL_INFO_CONTENT_TYPE` | `1048594` | Value of the Content-Type response header. |
| `curl.CURL_INFO_TOTAL_TIME` | `3145731` | Total time elapsed for the transfer in seconds. |
| `curl.CURL_INFO_SIZE_DOWNLOAD` | `3145736` | Size of downloaded data in bytes. |

---

## 5. SSL & Windows Certificate Caveats

On Windows, requests might fail due to lack of trusted certificates, throwing an error like:
`error adding trust anchors from file: .../ca-bundle.crt`
This happens when `libcurl` cannot access your OS certificate vault automatically.

#### **Solution 1 (Production - Secure):**
Download the official CA certificate bundle `cacert.pem` from curl's site and set its path:
```djazair
client.setopt(curl.CURL_OPT_CAINFO, "C:/path/to/cacert.pem")
```

#### **Solution 2 (Development - Bypassing verification):**
Disable peer and host SSL verification (Warning: do not use this in production):
```djazair
client.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
client.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
```

---

## 6. Advanced Recipes

### A. Downloading a File to Disk
This example shows how to download a file and write it to a local path using the standard `file` module:
```djazair
use curl
use file

let client = curl.Curl()
try
    client.setopt(curl.CURL_OPT_URL, "https://httpbin.org/image/png")
    client.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False) # Bypass SSL locally
    client.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    let imgData = client.perform()
    if !isNull(imgData) and client.getInfo(curl.CURL_INFO_RESPONSE_CODE) == 200
        file.write("downloaded_image.png", imgData)
        print("Image downloaded and saved successfully!")
    else
        print("Failed to download image: " + client.getError())
    end
catch e
    print("Error: " + str(e))
finally
    client.cleanup()
end
```

### B. Sending a POST Request with JSON Payload
```djazair
use curl

let client = curl.Curl()
try
    client.setopt(curl.CURL_OPT_URL, "https://httpbin.org/post")
    client.setopt(curl.CURL_OPT_POST, True)
    client.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    client.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    client.setHeaders([
        "Content-Type: application/json",
        "Accept: application/json"
    ])
    client.setopt(curl.CURL_OPT_POSTFIELDS, "{\"title\": \"Djazair Tutorial\", \"completed\": false}")
    
    let response = client.perform()
    if !isNull(response)
        print("HTTP Status: " + str(client.getInfo(curl.CURL_INFO_RESPONSE_CODE)))
        print("Response Body: " + response)
    else
        print("Failed: " + client.getError())
    end
catch e
    print("Error: " + str(e))
finally
    client.cleanup()
end
```

### C. Managing Cookies
Useful for maintaining session state between consecutive requests:
```djazair
use curl

let client = curl.Curl()
try
    client.setopt(curl.CURL_OPT_URL, "https://httpbin.org/cookies/set?session_token=dz123456")
    client.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    client.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # Enable cookie engine and save them to a file
    client.setopt(curl.CURL_OPT_COOKIEJAR, "./cookies.txt")
    client.setopt(curl.CURL_OPT_COOKIEFILE, "./cookies.txt")
    
    let res = client.perform()
    print("Set Cookie Response: " + res)
    
    # Second request will automatically read from cookies.txt and attach session details
    client.setopt(curl.CURL_OPT_URL, "https://httpbin.org/cookies")
    let res2 = client.perform()
    print("Sent Cookies Verified By Server: " + res2)
catch e
    print("Error: " + str(e))
finally
    client.cleanup()
end
```

### D. Connecting Through a Proxy
```djazair
use curl

let client = curl.Curl()
try
    client.setopt(curl.CURL_OPT_URL, "https://httpbin.org/get")
    client.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    client.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # Set proxy address and port
    client.setopt(curl.CURL_OPT_PROXY, "http://127.0.0.1:8080")
    
    # Optional: proxy authentication
    # client.setopt(curl.CURL_OPT_PROXYUSERPWD, "user:pass")
    
    let body = client.perform()
    if !isNull(body)
        print("Fetched via Proxy successfully!")
    else
        print("Proxy failed: " + client.getError())
    end
catch e
    print("Error: " + str(e))
finally
    client.cleanup()
end
```
