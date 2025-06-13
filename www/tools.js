const postJobButton = document.getElementById('post-job-button');
const deleteJobButton = document.getElementById('delete-job-button');
const getJobButton = document.getElementById('get-job-button');
const fileInput = document.getElementById('file-upload');
let postTargetPath = null;

const baseUrl = `${window.location.protocol}//${window.location.hostname}:${window.location.port}`;

function getStatusMessage(status) {
    const code = parseInt(status);
    const messages = {
        200: 'OK',
        201: 'Created',
        204: 'No Content',
        400: 'Bad Request',
        401: 'Unauthorized',
        403: 'Forbidden',
        404: 'Not Found',
        405: 'Method Not Allowed',
        413: 'Payload Too Large',
        500: 'Internal Server Error',
        502: 'Bad Gateway',
        503: 'Service Unavailable',
    };
    return messages[code] || '';
}

function showResponseInfo(status, headers, isError = false) {
    const panel = document.getElementById('response-panel');
    const statusElement = document.getElementById('response-status');
    const headersElement = document.getElementById('response-headers');

    // Set panel color based on success or error
    panel.classList.remove('hidden', 'bg-red-800', 'bg-gray-800');
    panel.classList.add(isError ? 'bg-red-800' : 'bg-gray-800');

    // Set status text
    statusElement.textContent = `Status: ${status} ${getStatusMessage(status)}`;
    statusElement.classList.toggle('text-red-400', isError);
    statusElement.classList.toggle('text-lime-300', !isError);

    // Prepare and animate headers
    let headersText = '';
    for (const [key, value] of headers.entries()) {
        headersText += `${key}: ${value}\n`;
    }

    headersElement.textContent = '';
    headersElement.classList.remove('typing');

    // Force reflow to restart animation
    void headersElement.offsetWidth;

    headersElement.textContent = headersText;
    headersElement.classList.add('typing');

    // Play pager sound
    // document.getElementById('pager-beep')?.play();
}


const handleFetchError = (err) => {
    console.error('Error:', err);
    // alert('An error occurred. Check the console for details.');
};

postJobButton.addEventListener('click', () => {
    fileInput.click();
});

getJobButton.addEventListener('click', async () => {
    console.log("Fetching path...");
    const path = prompt("Enter the path to fetch:");

    if (!path) {
        console.log('No path provided.');
        return;
    }

    try {
        const res = await fetch(`${baseUrl}/${path}`, {
            method: 'GET',
        });

        console.log("+++++++++++++++++++++++++++");
        console.log("Response Headers:");
        for (const [key, value] of res.headers.entries()) {
            console.log(`${key}: ${value}`);
        }
        console.log("+++++++++++++++++++++++++++");
        if (!res.ok) {
            showResponseInfo(res.status, res.headers, true);
            throw new Error(`Upload failed: ${res.status} ${res.statusText}`);
        } else
            showResponseInfo(res.status, res.headers, false);
    } catch (err) {
        handleFetchError(err);
    }
});

deleteJobButton.addEventListener('click', async () => {
    const path = prompt("Enter Route to Delete:");
    
    if (!path) {
        console.log('No path provided.');
        return;
    }

    try {
        const res = await fetch(`${baseUrl}/${path}`, {
            method: 'DELETE',
        });

        if (!res.ok) {
            showResponseInfo(res.status, res.headers, true);
            throw new Error(`Upload failed: ${res.status} ${res.statusText}`);
        } else
            showResponseInfo(res.status, res.headers, false);
        for (const [key, value] of res.headers.entries()) {
            console.log(`${key}: ${value}`);
        }
        console.log(`Path Deleted Successfully!`);
        // alert('Deleted successfully!');
    } catch (err) {
        handleFetchError(err);
    }
});

postJobButton.addEventListener('click', () => {
    const path = prompt("Enter the route to POST the file to (e.g., 'upload' or 'files/new'):");
    if (!path) {
        console.log('No path provided.');
        return;
    }

    postTargetPath = path;
    fileInput.click();
});

fileInput.addEventListener('change', async (e) => {
    const selectedFile = e.target.files[0];

    if (!selectedFile) {
        console.log('No file selected.');
        return;
    }

    if (!postTargetPath) {
        // alert('Upload path was not set. Please try again.');
        return;
    }

    console.log(`Uploading ${selectedFile.name} to /${postTargetPath}`);

    const formData = new FormData();
    formData.append('file', selectedFile);

    try {
        const res = await fetch(`${baseUrl}/${postTargetPath}`, {
            method: 'POST',
            body: formData,
        });

        if (!res.ok) {
            showResponseInfo(res.status, res.headers, true);
            throw new Error(`Upload failed: ${res.status} ${res.statusText}`);
        } else
            showResponseInfo(res.status, res.headers, false);

        for (const [key, value] of res.headers.entries()) {
            console.log(`${key}: ${value}`);
        }

        console.log('File uploaded successfully!');
        // alert('File uploaded successfully!');
       
    } catch (err) {
        handleFetchError(err);
    } finally {
        postTargetPath = null;
        fileInput.value = '';  // reset file input
    }
});
