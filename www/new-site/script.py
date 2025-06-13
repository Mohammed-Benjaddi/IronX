#!/usr/bin/env python3

import os
import cgi

print("Content-Type: text/html")
print()  # End of headers

# Read the form data
form = cgi.FieldStorage()

# Extract form fields (returns None if field doesn't exist)
first_name = form.getfirst("firstName", "")
last_name = form.getfirst("lastName", "")
email = form.getfirst("email", "")
method = os.environ.get("REQUEST_METHOD", "UNKNOWN")

# HTML Response
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Form Result</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 40px;
            background: #f9f9f9;
        }}
        .result {{
            padding: 20px;
            background: #fff;
            border: 1px solid #ddd;
            border-radius: 5px;
        }}
        h2 {{
            color: #333;
        }}
        .field {{
            margin-bottom: 10px;
        }}
        .field span {{
            font-weight: bold;
        }}
    </style>
</head>
<body>
    <div class="result">
        <h2>Form Submitted via {method}</h2>
        <div class="field"><span>First Name:</span> {first_name}</div>
        <div class="field"><span>Last Name:</span> {last_name}</div>
        <div class="field"><span>Email:</span> {email}</div>
    </div>
</body>
</html>
""")
