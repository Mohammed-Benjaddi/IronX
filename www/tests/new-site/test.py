#!/usr/bin/env python3

import os
import cgi

print("Content-Type: text/html")
print()  # End of headers

print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")

# Show some environment variables
print("<h2>Environment</h2>")
print("<pre>")
for key in sorted(os.environ.keys()):
    print(f"{key} = {os.environ[key]}")
print("</pre>")

# Handle GET query string
query = os.environ.get("QUERY_STRING", "")
print(f"<p><strong>Query string:</strong> {query}</p>")

# Handle POST data
if os.environ.get("REQUEST_METHOD", "") == "POST":
    form = cgi.FieldStorage()
    print("<h2>POST Data</h2>")
    for key in form.keys():
        print(f"<p>{key} = {form.getvalue(key)}</p>")

print("</body>")
print("</html>")
