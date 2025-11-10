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

    set<string> vocab;
    map<string, int> label_count;
    map<string, int> word_count;
    map<string, map<string, int>> word_label_count;

    public:
    void train(csvstream &training_file);
    string predict(string &content);
    set<string> unique_words(const string &str) {
      istringstream source(str);
      set<string> words;
      string word;
      while (source >> word) {
        words.insert(word);
      }
      return words;
    } 
};

double log_prior(int label_count, int total_count) {
  return log(static_cast<double>(label_count) / static_cast<double>(total_count));
}
void Classifier::train(csvstream &training_file) {
  map <string, string> row;

  cout << "training data:" << endl;
  vector<string> ignore = training_file.getheader();
  while(training_file >> row) {
    train_num += 1;
    string label = row["tag"];
    string content = row["content"];
    cout << "  label = " << label << ", content = " << content << endl;
    label_count[label] += 1;

    set<string> words = unique_words(content);
  
    for (const string &word : words) {
      vocab.insert(word);
      word_count[word] += 1;
      word_label_count[word][label] += 1;
    }

    vocab_size = vocab.size();
  }

  cout << "trained on " << train_num << " examples" << endl;
  cout << "vocabulary size = " << vocab_size << endl;
  cout << endl;

  cout << "classes:" << endl;
    for (auto &label_pair : label_count) {
      cout << "  " << label_pair.first << ", " << label_pair.second << " examples, "
      << "log-prior = " << log_prior(label_pair.second, train_num) << endl;
    }
  
  cout << "classifier parameters:" << endl;
  for (auto &label : label_count) {
    for (auto &word : vocab) {
      int count_wl = word_label_count[word][label.first];
      if (count_wl > 0)
      cout <<  "  " << label.first << ":" << word << ", count = " << count_wl 
      << ", log-likelihood = " << log_prior(count_wl, label.second) << endl;
    }
  }
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