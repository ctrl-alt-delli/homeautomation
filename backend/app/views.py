"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site 

from app import app, Config,  mongo, Mqtt
from flask import escape, render_template, request, jsonify, send_file, redirect, make_response, send_from_directory 
from json import dumps, loads 
from werkzeug.utils import secure_filename
from datetime import datetime,timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor
 



#####################################
#   Routing for your application    #
#####################################


# 1. CREATE ROUTE FOR '/api/set/combination'
@app.route('/api/set/combination', methods=['POST']) 
def set_combination():   
    try:
        form = request.form
        passcode = escape(form.get("passcode"))
        data = mongo.set_combination(passcode)
        if data:
            return jsonify({"status": "complete", "data": "complete"})
    except Exception as e:
        print(f"set_combination error: {str(e)}")
    return jsonify({"status": "failed", "data": "failed"})

# 2. CREATE ROUTE FOR '/api/check/combination'
@app.route('/api/check/combination', methods=['POST']) 
def check_combination():   
    try:
        form = request.form
        passcode = escape(form.get("passcode"))
        data = mongo.check_combination(passcode)
        if data:
            return jsonify({"status": "complete", "data": "complete"})
    except Exception as e:
        print(f"check_combination error: {str(e)}")
    return jsonify({"status": "failed", "data": "failed"})

# 3. CREATE ROUTE FOR '/api/update'
@app.route('/api/update', methods=['POST'])
def update():
    try:
        jsonDoc = request.get_json()
        timestamp = floor(datetime.now().timestamp())
        jsonDoc['timestamp'] = timestamp
        
        Mqtt.publish("620157584", dumps(jsonDoc))
        Mqtt.publish("620157584_sub", dumps(jsonDoc))

        print(f"MQTT: {jsonDoc}")

        item = mongo.insertData(jsonDoc)
        if item:
            return jsonify({"status": "complete", "data": "complete"})
    except Exception as e:
        print(f"update Error: {str(e)}")
    return jsonify({"status": "failed", "data": "failed"})

# 4. CREATE ROUTE FOR '/api/reserve/<start>/<end>'
@app.route('/api/reserve/<int:start>/<int:end>', methods=['GET'])
def get_reserve_radar(start, end):
    try:
        result = list(mongo.retrieve_radar(start, end))
        if result:
            return jsonify({"status": "success", "data": result})
    except Exception as e:
        print(f"get_reserve_radar() error: {str(e)}")
    return jsonify({"status": "failed", "data": "failed"})

# 5. CREATE ROUTE FOR '/api/avg/<start>/<end>'
@app.route('/api/avg/<int:start>/<int:end>', methods=['GET'])
def get_average_radar(start, end):
    '''Returns the average of the 'reserve' field, using all documents found between specified start and end timestamps'''
    try:
        result = list(mongo.average_radar(start, end))
        if result:
            return jsonify({"status": "success", "data": result})
    except Exception as e:
        print(f"get_average_radar() error: {str(e)}")
    return jsonify({"status": "failed", "data": "failed"})


 


###############################################################
# The functions below should be applicable to all Flask apps. #
###############################################################


@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also tell the browser not to cache the rendered page. If we wanted
    to we could change max-age to 600 seconds which would be 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.errorhandler(405)
def page_not_found(error):
    """Custom 404 page."""    
    return jsonify({"status": 404}), 404



