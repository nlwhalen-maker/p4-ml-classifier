#include <iostream>
#include <string>
#include <set>
#include <map>
#include <cmath>
#include "csvstream.hpp" 
using namespace std;


class Classifier {
    private:
    int train_num;
    int vocab_size;

    set<string> content;
    map<string, int> label_count;
    map<string, int> word_count;
    map<string, map<string, int>> word_label_count;

    public:
    void train(csvstream &training_file);

    string predict(string &content);
};

void Classifier::train(csvstream &training_file) {
  int post_num;
  int content_size;
  label_count;
  word_count;
  word_label_count;
  map <string, string> row;

  cout << "training data:" << endl;
  vector<string> ignore = training_file.getheader();
  while(training_file >> row) {
    int n = stoi(row["n"]);
    int unique_views = stoi(row["unique_views"]);
    string tag = row["tag"];
    string content = row["content"];
    cout << "label = " << tag << ", content = " << content << endl;

    ++post_num;
    ++label_count[tag];
    for(const string &word : unique_words(content)) {
      
    }
  }
}



double calculate_log_probability() {
    
}

set<string> unique_words(const string &str) {
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) {
    words.insert(word);
  }
  return words;
}

int main(int argc, char *argv[]) {
  cout.precision(3);

  if (argc != 2 && argc != 3) {
    cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
    return 1;
  }

  string training_file = argv[1];
  string testing_file = (argc == 3) ? argv[2] : "";

  csvstream training_stream(training_file);
  Classifier classifier;
  classifier.train(training_stream);
}