from microdot import Microdot, send_file

# setup webserver
app = Microdot()


@app.errorhandler(404)
def not_found(request):
    return 'Not found'


@app.route('/css')
def css(request):
    """css is used to link the css file.
    
    :return: css file
    """
    
    return send_file('./static/layout.css')


@app.route('/')
def accueil(request):
    response = send_file("./templates/accueil.html")
    return response


@app.route('/info')
def info(request):
    response = send_file("./templates/info.html")
    return response


@app.route('/settings')
def settings(request):
    response = send_file("./templates/settings.html")
    return response


@app.route('/mode-1', methods=['GET', 'POST'])
def mode1(request):
    
    if request.method == 'POST':
        print("received m-1 : ", request.form.get('on-off'))
        print("colors : ", request.form.get("colors"))
        print("light : ", request.form.get("light"))
    
    response = send_file("./templates/mode-1.html")
    return response


@app.route('/mode-2', methods=['GET', 'POST'])
def mode2(request):
    
    if request.method == 'POST':
        print("received m-2 : ", request.form.get('on-off'))
        print("min s : ", request.form.get('min-s'))
        print("max s : ", request.form.get('max-s'))
        print("colors : ", request.form.get("colors"))
        print("light : ", request.form.get("light"))
    
    response = send_file("./templates/mode-2.html")
    return response


@app.route('/mode-3', methods=['GET', 'POST'])
def mode3(request):
    
    if request.method == 'POST':
        print("received m-3 : ", request.form.get('on-off'))
        print("time : ", request.form.get('time'))
        print("colors : ", request.form.get("colors"))
    
    response = send_file("./templates/mode-3.html")
    return response
    

@app.before_request
def before_request(request):
    pass

        
if __name__ == "__main__":
    print("start server.")
    app.run(host="0.0.0.0", port=80, debug=True)