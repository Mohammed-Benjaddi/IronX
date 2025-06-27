#!/usr/bin/php-cgi
<?php
echo "Content-Type: text/html\r\n\r\n";

echo "<!DOCTYPE html><html><body>";
echo "<h1>PHP CGI Test</h1>";

// Show method
echo "<p><strong>Method:</strong> " . htmlspecialchars($_SERVER['REQUEST_METHOD']) . "</p>";

// GET parameters
if (!empty($_GET)) {
    echo "<h2>GET Parameters:</h2><ul>";
    foreach ($_GET as $k => $v) {
        echo "<li>" . htmlspecialchars($k) . ": " . htmlspecialchars($v) . "</li>";
    }
    echo "</ul>";
}

// POST parameters
if (!empty($_POST)) {
    echo "<h2>POST Parameters:</h2><ul>";
    foreach ($_POST as $k => $v) {
        echo "<li>" . htmlspecialchars($k) . ": " . htmlspecialchars($v) . "</li>";
    }
    echo "</ul>";
}

// File uploads
if (!empty($_FILES)) {
    echo "<h2>Uploaded Files:</h2><ul>";
    foreach ($_FILES as $name => $file) {
        echo "<li>" . htmlspecialchars($name) . ": " .
             "Name = " . htmlspecialchars($file['name']) . ", " .
             "Size = " . $file['size'] . " bytes, " .
             "Type = " . htmlspecialchars($file['type']) . "</li>";
    }
    echo "</ul>";
}

echo "</body></html>";
?>
