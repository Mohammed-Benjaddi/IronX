#!/usr/bin/env python

import os
import sys
import cgi
import cgitb
from datetime import datetime

# Enable CGI error reporting (optional - useful for debugging)
cgitb.enable()

def main():
    # Print Content-Type header (required!)
    print("Content-Type: text/html")
    print("")  # Empty line separates headers from body
    
    # Get environment variables set by the web server
    request_method = os.environ.get('REQUEST_METHOD', 'GET')
    query_string = os.environ.get('QUERY_STRING', '')
    content_length = os.environ.get('CONTENT_LENGTH', '0')
    
    # Parse form data
    form = cgi.FieldStorage()
    
    # Generate HTML response
    print("""<!DOCTYPE html>
<html>
<head>
    <title>CGI Test Script</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 800px; }
        .info { background: #f0f8ff; padding: 15px; border-radius: 5px; }
        .form-section { margin: 20px 0; padding: 20px; background: #f9f9f9; }
        input, textarea { margin: 5px 0; padding: 5px; }
        button { padding: 10px 20px; background: #007cba; color: white; border: none; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐍 CGI Python Script Test</h1>
        
        <div class="info">
            <h3>Request Information:</h3>
            <p><strong>Method:</strong> {method}</p>
            <p><strong>Query String:</strong> {query}</p>
            <p><strong>Content Length:</strong> {length}</p>
            <p><strong>Timestamp:</strong> {timestamp}</p>
        </div>""".format(
            method=request_method,
            query=query_string if query_string else "(none)",
            length=content_length if content_length != '0' else "(none)",
            timestamp=datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        ))
    
    # Display form data if any
    if form:
        print("""
        <div class="info">
            <h3>Form Data Received:</h3>""")
        
        if len(form) > 0:
            print("<ul>")
            for key in form.keys():
                value = form[key].value
                print("<li><strong>{}:</strong> {}</li>".format(key, value))
            print("</ul>")
        else:
            print("<p>No form data received</p>")
        
        print("</div>")
    
    # Interactive form for testing
    print("""
        <div class="form-section">
            <h3>Test Form (GET method):</h3>
            <form method="GET">
                <p>
                    <label>Name: </label>
                    <input type="text" name="name" placeholder="Enter your name">
                </p>
                <p>
                    <label>Message: </label>
                    <textarea name="message" placeholder="Enter a message"></textarea>
                </p>
                <button type="submit">Send GET Request</button>
            </form>
        </div>
        
        <div class="form-section">
            <h3>Test Form (POST method):</h3>
            <form method="POST">
                <p>
                    <label>Email: </label>
                    <input type="email" name="email" placeholder="Enter your email">
                </p>
                <p>
                    <label>Age: </label>
                    <input type="number" name="age" placeholder="Enter your age">
                </p>
                <button type="submit">Send POST Request</button>
            </form>
        </div>
        
        <div class="info">
            <h3>Environment Variables:</h3>
            <ul>""")
    
    # Show some useful environment variables
    env_vars = [
        'SERVER_SOFTWARE', 'GATEWAY_INTERFACE', 'SERVER_PROTOCOL',
        'REQUEST_URI', 'SCRIPT_NAME', 'HTTP_USER_AGENT', 'HTTP_HOST'
    ]
    
    for var in env_vars:
        value = os.environ.get(var, '(not set)')
        print("<li><strong>{}:</strong> {}</li>".format(var, value))
    
    print("""
            </ul>
        </div>
        
        <div class="info">
            <h3>🎉 Success!</h3>
            <p>Your CGI script is working correctly! The C++ web server successfully:</p>
            <ul>
                <li>✅ Executed this Python script</li>
                <li>✅ Set up environment variables</li>
                <li>✅ Captured the output</li>
                <li>✅ Returned it as an HTTP response</li>
            </ul>
        </div>
    </div>
</body>
</html>""")

if __name__ == "__main__":
    main()