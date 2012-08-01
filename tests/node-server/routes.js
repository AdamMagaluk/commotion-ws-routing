module.exports = function(app,settings){

    app.all('/', function(req, res) {
        res.render('index');
    });

// If all fails, hit em with the 404
app.all('*', function(req, res){
    throw new NotFound;
});


};