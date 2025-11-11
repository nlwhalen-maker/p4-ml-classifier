#include <iostream>
#include <string>
#include <set>
#include <map>
#include <cmath>
#include "csvstream.hpp" 
using namespace std;


double log_probability(int label_count, int total_count) {
  return log(static_cast<double>(label_count) / static_cast<double>(total_count));
}

class Classifier {
    private:
    int train_num;
    int vocab_size;

    set<string> vocab;
    map<string, int> label_count;
    map<string, int> word_count;
    map<string, map<string, int>> word_label_count;

    public:
    void train(csvstream &training_file, bool do_output);
    pair<string, double> predict(string &content);
    void test(csvstream &testing_file);
    set<string> unique_words(const string &str);
};

void Classifier::train(csvstream &training_file, bool do_output) {
  train_num = 0;
  vocab_size = 0;
  vocab.clear();
  label_count.clear();
  word_count.clear();
  word_label_count.clear();

  map <string, string> row;

  if(do_output) {cout << "training data:" << endl;}
  
  vector<string> ignore = training_file.getheader();
  while(training_file >> row) {
    train_num += 1;
    string label = row["tag"];
    string content = row["content"];
    if (do_output) {
      cout << "  label = " << label << ", content = " << content << endl;
    }
    label_count[label] += 1;

    set<string> words = unique_words(content);
  
    for (const string &word : words) {
      vocab.insert(word);
      word_count[word] += 1;
      word_label_count[word][label] += 1;
    }

    vocab_size = vocab.size();
  }

  if (do_output) {
    cout << "trained on " << train_num << " examples" << endl;
    cout << "vocabulary size = " << vocab_size << endl;
    cout << endl;

    cout << "classes:" << endl;
      for (auto &label_pair : label_count) {
        cout << "  " << label_pair.first << ", " << label_pair.second << " examples, "
        << "log-prior = " << log_probability(label_pair.second, train_num) << endl;
      }
  
    cout << "classifier parameters:" << endl;
    for (auto &label : label_count) {
      for (auto &word : vocab) {
        int count_wl = word_label_count[word][label.first];
        if (count_wl > 0)
        cout <<  "  " << label.first << ":" << word << ", count = " << count_wl 
        << ", log-likelihood = " << log_probability(count_wl, label.second) << endl;
      }
    }
    cout << endl;
  }
}

pair<string, double> Classifier::predict(string &content) {
  set<string> words = unique_words(content);
  string best_label;
  double best_score = -INFINITY;

  for (auto &label_pair : label_count) {
    double log_prob = log_probability(label_pair.second, train_num);


    for (auto &word : words) {
      int count_wl = word_label_count[word][label_pair.first];
      int count_w = word_count[word];
      double log_likelihood = 0;

      if (count_wl > 0) {
        log_likelihood = log_probability(count_wl, label_pair.second);
      }
      else if (count_w > 0) {
        log_likelihood = log_probability(count_w, train_num);
      }
      else {
        log_likelihood = log_probability(1.0, train_num);
      }

      log_prob += log_likelihood;
    }

    if (log_prob > best_score) {
      best_label = label_pair.first;
      best_score = log_prob;
    }
  }
  
  pair<string, double> best_map(best_label, best_score);
  return best_map;
}

void Classifier::test(csvstream &testing_file) {
  int test_num = 0;
  int correct_counter = 0;
  map <string, string> row;

  cout << "trained on " << train_num << " examples" << endl << endl
  << "test data:" << endl;
  
  vector<string> ignore = testing_file.getheader();
  while(testing_file >> row) {
    test_num += 1;
    string label = row["tag"];
    string content = row["content"];
    pair<string, double> prediction = predict(content);

    cout << "  correct = " << label << ", predicted = " << prediction.first 
    << ", log-probability score = " << prediction.second << endl
    << "  content = " << content << endl << endl;

    if (label == prediction.first) {
      ++correct_counter;
    }
  }

  cout << "performance: " << correct_counter << " / " 
  << test_num << " posts predicted correctly" << endl;
}

set<string> Classifier::unique_words(const string &str) {
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

  csvstream training_stream(training_file);

  Classifier classifier;

  if (argc == 2) {
    try {
      classifier.train(training_stream, true);
    }
    catch (csvstream_exception &e) {
      cout << "Error opening file: " << training_file << endl;
    }
  }
  else {
    string testing_file = argv[2];
    try {
      csvstream testing_stream(testing_file);
      classifier.train(training_stream, false);
      classifier.test(testing_stream);
    }
    catch (csvstream_exception &e) {
      cout << "Error opening file: " << testing_file << endl;
    }
  }
}