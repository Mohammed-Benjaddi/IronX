
// main variables

let theInput = document.querySelector('.get-repos input');
let getButton = document.querySelector('.get-repos .get-button');
let reposData = document.querySelector('.show-data');

getButton.onclick = function () {
    getRepos();
}

// get Repos function
function getRepos() {

    if (theInput.value == "") {
        
        reposData.innerHTML = "<sapn>Please write github username</sapn>";

    } else {

        fetch(`https://api.github.com/users/${theInput.value}/repos`)
            .then((res) => res.json())

            .then((repos) => {
                // empty the container
                reposData.innerHTML = '';

                // loop on repositories
                repos.forEach(repo => {

                    // create the main div
                    let mainDiv = document.createElement('div');
                    
                    // create repo name text
                    let repoName = document.createTextNode(repo.name);
                    
                    // append the text to main div
                    mainDiv.appendChild(repoName);

                    // create repo URL anchor
                    let theUrl = document.createElement("a");

                    // create repo URL text
                    let theUrlText = document.createTextNode("visit");

                    // append url text to anchor tag
                    theUrl.appendChild(theUrlText);

                    // add the hypertext Refernce 'href'
                    theUrl.href = `https://github.com/${theInput.value}/${repo.name}`;

                    // set Attribute blank
                    theUrl.setAttribute('target', '_blank');

                    // append URL anchor to main div
                    mainDiv.appendChild(theUrl);

                    // create stars count span
                    let starsSpan = document.createElement("span");

                    // create stars count span
                    let starsText = document.createTextNode(`Stars ${repo.stargazers_count}`);

                    // add stars count to stars span
                    starsSpan.appendChild(starsText);

                    // append stars to main div
                    mainDiv.appendChild(starsSpan);

                    // add class to main mainDiv
                    mainDiv.className = 'repo-box';

                    // append the main div to container
                    reposData.appendChild(mainDiv)


                });
        })
    }

}

// let simo = document.getElementsByClassName('simo')[0];
// console.log(simo);