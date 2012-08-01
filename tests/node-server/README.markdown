# Easy, Breezy Node.js Hosting & Deployment

Deploying a Node.js app to Heroku is as simple as this:

    git push heroku master


## Resources

* [GitHub](https://github.com/)
* [NVM (Node Version Manager)](https://github.com/creationix/nvm#readme)
* [NPM (Node Package Manager)](http://npmjs.org/)
* [Node](http://nodejs.org/)
* [Getting Started with Node.js on Heroku/Cedar](http://devcenter.heroku.com/articles/node-js)
* [Express - node web framework](http://expressjs.com/)
* [Backbone.js](http://documentcloud.github.com/backbone/)

## 1. Install Node, NVM, and NPM

Pre-requisites

* git
* bash
* Xcode, gcc, or other C++ compiler

Install NVM https://github.com/creationix/nvm#readme

    git clone git://github.com/creationix/nvm.git ~/.nvm
    
Add this line to `~/.bashrc` or `~/.profile`

    . ~/.nvm/nvm.sh

Install Node v0.4.7. **Important**: Heroku's Cedar stack supports Node v0.4.7 only; none sooner, none later.

    nvm install v0.4.7
    # downloads and compiles Node
    nvm alias default v0.4.7
    nvm use v0.4.7
    node --version
    # v0.4.7

Install NPM

    curl http://npmjs.org/install.sh | sh

## 2. Install Heroku CLI

Pre-requisites:

* Heroku account - https://api.heroku.com/signup
* Ruby

Download Heroku CLI

* http://devcenter.heroku.com/articles/node-js#local_workstation_setup

Login to Heroku. You only have to do this once.

    heroku login

## 3. Hello Node

Clone Meetup project

    git clone git://github.com/toolbear/easy-breezy-node-heroku.git
    cd easy-breezy-node-heroku

Install Node modules required for project

    npm install

NPM installs dependencies for our project that are declared in `package.json`. Each time we add a dependency
to `package.json` we will re-run `npm install`.

Run locally

    foreman start
    # 16:29:15 web.1     | started with pid 35445
    # 16:29:15 web.1     | Listening on 5000

Visit [http://localhost:5000/](http://localhost:5000/)

## 4. Review Node.js Project Structure

    easy-breezy-node-heroku
    ├── .git
    │   ├── ...
    ├── .gitignore
    ├── Procfile
    ├── Procfile
    ├── README.markdown
    ├── package.json
    └── web.js

### Excercise

* Change "Hello World" to "Hello, {You}", e.g. "Hello, toolbear"
* Restart `foreman`, see your change locally
* Commit your changes
        git add web.js
        git ci -m 'personalized greeting'

## 5. Deploy!

Create new Heroku app in your account. You do this once per app. **Important**: specifying Cedar is important. The original Heroku stack was Ruby only.

    heroku create --stack cedar
    # Creating sharp-rain-871... done, stack is cedar
    # http://sharp-rain-871.herokuapp.com/ | git@heroku.com:sharp-rain-871.git
    # Git remote heroku added
    
Check out the placeholder app:

    heroku open

Deploy your code:

    git push heroku master
    # ...

Your app!

    heroku open
    
## 6. A little front-end

Add in `express.static(...)` for static file serving.

Exercise

* Change `Procfile` to point to `web.static.js`
* restart Foreman and test out locally
* Change the message displayed and test locally
* Deploy to heroku
* test remotely

## 7. REST resource

Exercise

* Change `Procfile` to point to `web.backbone.js`
* test locally
* Change mock database, add your user
* test locally with your user
* Deploy to heroku
* test remotely

Testing REST services with `curl`

    # to fetch user with username "toolbear"
    curl -i http://localhost:5000/user/toolbear

## 8. Debugging remotely

    heroku logs --tail
    # 2012-01-19T21:45:30+00:00 heroku[api]: Add-on add logging:basic by tim@tool-man.org
    # 2012-01-19T21:45:30+00:00 heroku[api]: Release v2 created by tim@tool-man.org
    # 2012-01-19T21:46:22+00:00 heroku[slugc]: Slug compilation started
    # ...


## 9. Have some Backbone.js

Exercise for the reader:

* Add [Backbone.js](http://documentcloud.github.com/backbone/) to `public/js` and `public/login.html`
* Create User model object
* Wire up username field to User model
* Add a view for User object
* Sync user model with REST resource when username field updated, re-render User view
* ???
* Profit!

