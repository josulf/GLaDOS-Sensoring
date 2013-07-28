const char http_http[8] = 
/* "http://" */
{0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0 };
const char http_200[5] = 
/* "200 " */
{0x32, 0x30, 0x30, 0x20, 0 };
const char http_301[5] = 
/* "301 " */
{0x33, 0x30, 0x31, 0x20, 0 };
const char http_302[5] = 
/* "302 " */
{0x33, 0x30, 0x32, 0x20, 0 };
const char http_get[5] = 
/* "GET " */
{0x47, 0x45, 0x54, 0x20, 0 };
const char http_post[6] =
/* "POST " */
{0x50, 0x4f, 0x53, 0x54, 0x20, 0 };
const char http_10[9] = 
/* "HTTP/1.0" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0 };
const char http_11[9] = 
/* "HTTP/1.1" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31, 0 };
const char http_content_length[16] =
/* "Content-Length:" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x4C, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3a, 0 };
const char http_content_type[15] = 
/* "content-type: " */
{0x63, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0 };
const char http_texthtml[10] = 
/* "text/html" */
{0x74, 0x65, 0x78, 0x74, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0 };
const char http_location[11] = 
/* "location: " */
{0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0 };
const char http_host[7] = 
/* "host: " */
{0x68, 0x6f, 0x73, 0x74, 0x3a, 0x20, 0 };
const char http_crnl[3] = 
/* "\r\n" */
{0xd, 0xa, 0 };
const char http_index_html[12] = 
/* "/index.html" */
{0x2f, 0x69, 0x6e, 0x64, 0x65, 0x78, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0 };
const char http_404_html[10] = 
/* "/404.html" */
{0x2f, 0x34, 0x30, 0x34, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0 };
const char http_referer[9] = 
/* "Referer:" */
{0x52, 0x65, 0x66, 0x65, 0x72, 0x65, 0x72, 0x3a, 0 };
const char http_header_200[84] = 
/* "HTTP/1.0 200 OK\r\nServer: uIP/1.0 http://www.sics.se/~adam/uip/\r\nConnection: close\r\n" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x32, 0x30, 0x30, 0x20, 0x4f, 0x4b, 0xd, 0xa, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 0x75, 0x49, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 0x64, 0x61, 0x6d, 0x2f, 0x75, 0x69, 0x70, 0x2f, 0xd, 0xa, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 0xd, 0xa, 0 };
const char http_header_404[91] = 
/* "HTTP/1.0 404 Not found\r\nServer: uIP/1.0 http://www.sics.se/~adam/uip/\r\nConnection: close\r\n" */
{0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x34, 0x30, 0x34, 0x20, 0x4e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0xd, 0xa, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x3a, 0x20, 0x75, 0x49, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 0x64, 0x61, 0x6d, 0x2f, 0x75, 0x69, 0x70, 0x2f, 0xd, 0xa, 0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 0xd, 0xa, 0 };
const char http_content_type_plain[29] = 
/* "Content-type: text/plain\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_html[28] = 
/* "Content-type: text/html\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_css [27] = 
/* "Content-type: text/css\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x63, 0x73, 0x73, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_text[28] = 
/* "Content-type: text/text\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x74, 0x65, 0x78, 0x74, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_png [28] = 
/* "Content-type: image/png\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x2f, 0x70, 0x6e, 0x67, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_gif [28] = 
/* "Content-type: image/gif\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x2f, 0x67, 0x69, 0x66, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_jpg [29] = 
/* "Content-type: image/jpeg\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x2f, 0x6a, 0x70, 0x65, 0x67, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_content_type_binary[43] = 
/* "Content-type: application/octet-stream\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2f, 0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0xd, 0xa, 0xd, 0xa, 0 };
const char http_html[6] = 
/* ".html" */
{0x2e, 0x68, 0x74, 0x6d, 0x6c, 0 };
const char http_shtml[7] = 
/* ".shtml" */
{0x2e, 0x73, 0x68, 0x74, 0x6d, 0x6c, 0 };
const char http_htm[5] = 
/* ".htm" */
{0x2e, 0x68, 0x74, 0x6d, 0 };
const char http_css[5] = 
/* ".css" */
{0x2e, 0x63, 0x73, 0x73, 0 };
const char http_png[5] = 
/* ".png" */
{0x2e, 0x70, 0x6e, 0x67, 0 };
const char http_gif[5] = 
/* ".gif" */
{0x2e, 0x67, 0x69, 0x66, 0 };
const char http_jpg[5] = 
/* ".jpg" */
{0x2e, 0x6a, 0x70, 0x67, 0 };
const char http_text[5] = 
/* ".txt" */
{0x2e, 0x74, 0x78, 0x74, 0 };
const char http_txt[5] = 
/* ".txt" */
{0x2e, 0x74, 0x78, 0x74, 0 };
const char http_user_agent_fields[77] = 
/* "Connection: close\r\nUser-Agent: uIP/1.0 (; http://www.sics.se/~adam/uip/)\r\n\r\n" */
{0x43, 0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65, 0xd, 0xa, 0x55, 0x73, 0x65, 0x72, 0x2d, 0x41, 0x67, 0x65, 0x6e, 0x74, 0x3a, 0x20, 0x75, 0x49, 0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x28, 0x3b, 0x20, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x73, 0x69, 0x63, 0x73, 0x2e, 0x73, 0x65, 0x2f, 0x7e, 0x61, 0x64, 0x61, 0x6d, 0x2f, 0x75, 0x69, 0x70, 0x2f, 0x29, 0xd, 0xa, 0xd, 0xa, 0 };
