#!/usr/bin/env python3

import os
import sys
import urllib.parse
from datetime import datetime

def parse_form_data():
    """Parse form data from GET or POST request"""
    request_method = os.environ.get('REQUEST_METHOD', 'GET')
    form_data = {}
    
    if request_method == 'GET':
        # GET data comes from QUERY_STRING
        query_string = os.environ.get('QUERY_STRING', '')
        if query_string:
            form_data = urllib.parse.parse_qs(query_string)
    
    elif request_method == 'POST':
        # POST data comes from stdin
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length > 0:
            post_data = sys.stdin.read(content_length)
            form_data = urllib.parse.parse_qs(post_data)
    
    # parse_qs returns lists, so get first value for each field
    parsed_data = {}
    for key, value_list in form_data.items():
        parsed_data[key] = value_list[0] if value_list else ''
    
    return parsed_data, request_method

def generate_response(form_data, method):
    """Generate HTML response"""
    
    # Get form fields
    first_name = form_data.get('firstName', '')
    last_name = form_data.get('lastName', '')
    email = form_data.get('email', '')
    
    # Check if we have data
    has_data = first_name or last_name or email
    
    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Form Response</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            max-width: 600px;
            margin: 50px auto;
            padding: 20px;
            background-color: #f5f5f5;
        }}
        .response-container {{
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }}
        .success {{
            background-color: #d4edda;
            color: #155724;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
            border: 1px solid #c3e6cb;
        }}
        .info {{
            background-color: #e2e3e5;
            color: #383d41;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
            border: 1px solid #d6d8db;
        }}
        .data-table {{
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
        }}
        .data-table th, .data-table td {{
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }}
        .data-table th {{
            background-color: #f8f9fa;
            font-weight: bold;
        }}
        .back-link {{
            display: inline-block;
            margin-top: 20px;
            padding: 10px 20px;
            background-color: #007bff;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            transition: background-color 0.3s;
        }}
        .back-link:hover {{
            background-color: #0056b3;
        }}
        .method-badge {{
            display: inline-block;
            padding: 4px 8px;
            background-color: {('#28a745' if method == 'GET' else '#dc3545')};
            color: white;
            border-radius: 3px;
            font-size: 12px;
            font-weight: bold;
        }}
    </style>
</head>
<body>
    <div class="response-container">
        <h1>Form Submission Response</h1>
        
        <div class="info">
            <strong>Method Used:</strong> <span class="method-badge">{method}</span><br>
            <strong>Timestamp:</strong> {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}
        </div>"""

    if has_data:
        html += f"""
        <div class="success">
            <strong>✅ Form submitted successfully!</strong>
        </div>
        
        <h2>Received Data:</h2>
        <table class="data-table">
            <tr>
                <th>Field</th>
                <th>Value</th>
            </tr>
            <tr>
                <td><strong>First Name:</strong></td>
                <td>{first_name if first_name else '<em>Not provided</em>'}</td>
            </tr>
            <tr>
                <td><strong>Last Name:</strong></td>
                <td>{last_name if last_name else '<em>Not provided</em>'}</td>
            </tr>
            <tr>
                <td><strong>Email:</strong></td>
                <td>{email if email else '<em>Not provided</em>'}</td>
            </tr>
        </table>
        
        <h2>Personal Greeting:</h2>
        <div style="background: #f8f9fa; padding: 20px; border-radius: 5px; border-left: 4px solid #007bff;">"""
        
        if first_name and last_name:
            html += f"<h3>Hello, {first_name} {last_name}! 👋</h3>"
        elif first_name:
            html += f"<h3>Hello, {first_name}! 👋</h3>"
        else:
            html += "<h3>Hello there! 👋</h3>"
            
        if email:
            html += f"<p>We have your email on file as: <strong>{email}</strong></p>"
            html += "<p>Thank you for providing your contact information!</p>"
        
        html += """
        </div>"""
    else:
        html += """
        <div class="info">