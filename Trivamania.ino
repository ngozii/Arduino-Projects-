//Trivmainia is a mini game that asks trivia questions.
//You input your answers and the code checks to see if it is correct.
//Each correct answer earns a point and each wrong answer earns a strike
//If you have three strikes, you lose.
//The game returns a comment at the end showing your score.

void setup() {
  Serial.begin(9600);
  //Intro Message
  Serial.println("Welcome to Trivmania!");
  delay(2500);
  Serial.println("The rules of the game are simple");
  delay(1500);
  Serial.println("You have 5 questions to answer");
  Serial.println("If you answer correctly, you earn a point");
  Serial.println("If you answer incorrectly, you get a strike.");
  delay(2000);
  Serial.println("If you get three strikes you're out");
  delay(1500);
  Serial.println("Ready?");
  delay(2500);
  Serial.println("Let's go!");
  delay(2500);
}

void loop() {


  //Question structure to pair each question with its correct answer
  struct Question {
    String text;
    String answer;
  };
  Question questions[] = {
    { "What animal is known as 'the ship of the desert'?", "camel" },
    { "What continent is Turkey in?", "europe" },
    { "How many great grandparents do you have in your lineage?", "8" },
    { "What is the tallest land animal in Asia?", "elephant" },
    { "What is the median of this set of numbers: 8, 4, 9?", "8" }
  };

  //initialize game variables
  int strikes = 0;
  int score = 0;
  int total_questions = sizeof(questions) / sizeof(questions[0]);

  //Main loop for the game

  for (int i = 0; i < total_questions; i++)  //loops through each question
  {
    Serial.println("Question" + String(i + 1) + ": " + questions[i].text);

    while (Serial.available() == 0) { //waits for user input. Blocks until something is entered
      delay(500);
    }
    String userInput = Serial.readStringUntil('\n');  //Reads and stores the user's input
    userInput.trim();                                 //Clean user input for comparison
    userInput.toLowerCase();                          //Ensure case insensitive matching 

    if (userInput == questions[i].answer) {
      Serial.println("Correct");  //Response for correct answer
      score++;
    } else {
      Serial.println("Wrong! The correct answer was; " + questions[i].answer);  //Response for wrong answer
      strikes++;
    
  

      if (strikes == 3) { //Checks if player has reached strike limit; ends game immediately
        Serial.println("Game Over! You got 3 strikes.");  //Ends game when strikes get to 3
        Serial.println("Your final score is: " + String(score) + " out of " + String(total_questions));
        return;
      }
    }
  }
  delay(2000);
  Serial.println("You finished the game!");
  Serial.println("Your final score is: " + String(score) + " out of " + String(total_questions));  //displays score

  // Give a performance comment
  if (score == total_questions) {
    Serial.println("You're pretty much a genius!");
  } else if (score >= total_questions - 1) {
    Serial.println("Not bad, well done!");
  } else if (score >= total_questions / 2) {
    Serial.println("Do better next time!");
  } else {
    Serial.println("Better luck next time!");
  }


  delay(30000);  // Wait  30 seconds before restarting to prevent accidental replays
}
