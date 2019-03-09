# TriviaGame
Utilizes IoT and sqlite3 to create a Trivia Game with lasting memory
Code in Python and C++

Parts: Esp32, tft Screen 1.8", 2 buttons

I created two databases in order to do this design exercise, one that logs the question, correct answer, & time, and one that stores the total score & time. Having both log the time when logging the other info is useful as it allows me to easily look up the most recent correct answer corresponding to the question that ha been asked most recently when I check the user's answer to the correct answer after I have already done a GET request to display the question on the screen. I also made use of a POST request in order to send the user's answer input to the python server code so that the python code can check the user's answer against the correct one for the current question and update the total score accordingly. At this time is also sends a response back to the arduino that displays to the user whether or not they got the question correct along with the current total score. The arduino code only deals with the display, and user interface aspects. 

https://www.youtube.com/watch?v=sDOSd2ib9LE to see it in action!!
