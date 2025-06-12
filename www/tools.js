const postJobButton = document.getElementById('post-job-button');
const deleteJobButton = document.getElementById('delete-job-button');
const getJobButton = document.getElementById('get-job-button');
const fileInput = document.getElementById('file-upload');
let postTargetPath = null;

const baseUrl = `${window.location.protocol}//${window.location.hostname}:${window.location.port}`;

const handleFetchError = (err) => {
    console.error('Error:', err);
    alert('An error occurred. Check the console for details.');
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
            throw new Error(`Failed to delete: ${res.status} ${res.statusText}`);
        }
        for (const [key, value] of res.headers.entries()) {
            console.log(`${key}: ${value}`);
        }
        console.log(`Path Deleted Successfully!`);
        alert('Deleted successfully!');
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
        alert('Upload path was not set. Please try again.');
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
            throw new Error(`Upload failed: ${res.status} ${res.statusText}`);
        }

        for (const [key, value] of res.headers.entries()) {
            console.log(`${key}: ${value}`);
        }

        console.log('File uploaded successfully!');
        alert('File uploaded successfully!');
    } catch (err) {
        handleFetchError(err);
    } finally {
        postTargetPath = null;
        fileInput.value = '';  // reset file input
    }
});
