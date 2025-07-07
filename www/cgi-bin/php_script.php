#!/usr/bin/env php-cgi
<?php
echo "Content-Type: text/html\r\n\r\n";

$method = $_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN';

$firstName = '';
$lastName = '';
$email = '';

if ($method === 'GET') {
    // Data is in QUERY_STRING
    parse_str($_SERVER['QUERY_STRING'] ?? '', $params);
    $firstName = htmlspecialchars($params['firstName'] ?? '');
    $lastName  = htmlspecialchars($params['lastName'] ?? '');
    $email     = htmlspecialchars($params['email'] ?? '');
} elseif ($method === 'POST') {
    $input = file_get_contents('php://stdin');
    parse_str($input, $params);
    $firstName = htmlspecialchars($params['firstName'] ?? '');
    $lastName  = htmlspecialchars($params['lastName'] ?? '');
    $email     = htmlspecialchars($params['email'] ?? '');
} else {
    echo "<h1>Unsupported Method</h1>";
    exit;
}

echo "<!DOCTYPE html><html><body>";
echo "<h2>Form Submitted via $method</h2>";
echo "<p><strong>First Name:</strong> $firstName</p>";
echo "<p><strong>Last Name:</strong> $lastName</p>";
echo "<p><strong>Email:</strong> $email</p>";
echo "</body></html>";
?>
