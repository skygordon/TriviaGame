
import requests
import json
import datetime
import sqlite3

visits_db1 = '__HOME__/trivia.db'
trivia_db = 'trivia.db' # name of database1
def create_database1():
    conn = sqlite3.connect(visits_db1)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS trivia_table (question text, corans int, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close() # close connection to database
 
create_database1()  #call the function!


visits_db2 = '__HOME__/score.db'
score_db = 'score.db' # name of database2
def create_database2():
    conn = sqlite3.connect(visits_db2)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # make cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS score_table (score int, timing timestamp);''') # run a CREATE TABLE command
    conn.commit() # commit commands
    conn.close() # close connection to database
 
create_database2()  #call the function!

def insertstart():
    conn = sqlite3.connect(visits_db2)
    c = conn.cursor()
    c.execute('''INSERT into score_table VALUES (?,?);''', (0 , datetime.datetime.now()))
    conn.commit()
    conn.close()


def request_handler(request):
    if (request['method']=="GET"):
        r = requests.get("http://opentdb.com/api.php?amount=1&category=9&difficulty=easy&type=boolean")
        response = json.loads(r.text)
        question = str(response["results"][0]["question"])
        correctans = str(response["results"][0]["correct_answer"])
#        converts correct answer into int value that will make it easier to check later on after user inputs their answer
        if correctans=='True':
            corans = 1; #one button press for True is correct
        if correctans=='False':
            corans = 2; #two button presses for False is correct
# inserts in newest trivia question and correct answer
        conn = sqlite3.connect(visits_db1)
        c = conn.cursor()
        c.execute('''INSERT into trivia_table VALUES (?,?,?);''',(question, corans, datetime.datetime.now()))
        conn.commit()
        conn.close()
#        returns the trivia question to arduino
        return question
    
    
    elif (request['method']=="POST"):
        if len(request['values'])!= 0:
            ans = int(request['values']['numcount'])
        else:
            ans = int(request['form']['numcount'])
#        Looks up correct answer from most recent question
        conn = sqlite3.connect(visits_db1)
        c = conn.cursor()
        correct = c.execute('''SELECT corans FROM trivia_table ORDER BY timing DESC LIMIT 1;''').fetchall()
        conn.commit()
        conn.close()
        correctanswer = int(correct[0][0])
#        checks to see if user's answer is correct
        if correctanswer == ans:
#            if correct, looks up current total score and adds 1
            conn = sqlite3.connect(visits_db2)
            c = conn.cursor()
            scor = c.execute('''SELECT score FROM score_table ORDER BY timing DESC LIMIT 1;''').fetchall()
            totalscore = int(scor[0][0])
            conn.commit()
            conn.close()
            totalscore = totalscore+1
#            then enters the new total score into database to be accessed next time
            conn = sqlite3.connect(visits_db2)
            c = conn.cursor()
            c.execute('''INSERT into score_table VALUES (?,?);''',(totalscore, datetime.datetime.now()))
            conn.commit()
            conn.close()
            return 'You Are Correct! The New Total Score is {}'.format(totalscore)
            
        elif correctanswer != ans:
            conn = sqlite3.connect(visits_db2)
            c = conn.cursor()
            scor = c.execute('''SELECT score FROM score_table ORDER BY timing DESC LIMIT 1;''').fetchall()
            conn.commit()
            conn.close()
            return 'Whoops! You Are Wrong... The Total Score is {}'.format(scor[0][0])



"""
Created on Thu Mar  7 20:31:12 2019

@author: skylargordon
"""
