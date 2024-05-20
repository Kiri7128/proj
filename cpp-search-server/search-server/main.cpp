#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string read_line() {
    string s;
    getline(cin, s);
    return s;
}

int read_line_with_number() {
    int result = 0;
    cin >> result;
    cin.ignore(); // Consume the newline character
    return result;
}

vector<string> split_into_words(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void set_stop_words(const string& text) {
        for (const string& word : split_into_words(text)) {
            stop_words_.insert(word);
        }
    }

    void add_document(int document_id, const string& document) {
        const vector<string> &words = split_into_words_no_stop(document);
        double tf = 1.0 / static_cast<double>(words.size());
        // Count the occurrences of each word in the document
        for (const string& word : words) {
            word_to_documents_[word][document_id] += tf;
        }
        document_count_++;
    }

    vector<Document> find_top_documents(const string& raw_query) const {
        const Query query = parse_query(raw_query);
        const auto matched_documents = find_all_documents(query);

        // Sort the matched documents based on relevance
        vector<Document> sorted_documents(matched_documents.begin(), matched_documents.end());
        sort(sorted_documents.begin(), sorted_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });

        // Limit the number of result documents
        if (sorted_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            sorted_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return sorted_documents;
    }

private:
    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    map<string, map<int, double>> word_to_documents_;

    int document_count_ = 0;
    set<string> stop_words_;

    bool is_stop_word(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> split_into_words_no_stop(const string& text) const {
        vector<string> words;
        for (const string& word : split_into_words(text)) {
            if (!is_stop_word(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    QueryWord parse_query_word(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, is_stop_word(text) };
    }

    double count_idf(double word_count) const
    {
        return log(static_cast<double>(document_count_) / word_count);
    }

    Query parse_query(const string& text) const {
        Query query;
        for (const string& word : split_into_words(text)) {
            const QueryWord query_word = parse_query_word(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    vector<Document> find_all_documents(const Query& query) const {
        map<string, double> idf;
        map<int, double> rel_doc;
        vector<Document> matched_documents;
        for (const string& word : query.plus_words) {
            if (word_to_documents_.count(word) > 0) {
                idf[word] = count_idf(static_cast<double>(word_to_documents_.at(word).size()));
            }
        }

        for (const string& word : query.plus_words) {
            if (word_to_documents_.count(word) > 0) {
                for (const auto& [document_id,tf] : word_to_documents_.at(word)) {
                    rel_doc[document_id] = tf * idf[word] + rel_doc[document_id];
                }
            }
        }
        set<int> index_of_minuswords;
        for (const string& word : query.minus_words) {
            if (word_to_documents_.count(word) > 0) {
                for (auto a : word_to_documents_.at(word)) {
                    index_of_minuswords.insert(a.first);
                }
            }
        }

        for (auto number : rel_doc) {
            int doc_id = number.first;
            double rel = number.second;
            if (index_of_minuswords.count(doc_id) == 0) {
                matched_documents.push_back({ doc_id,rel });
            }
        }
        return matched_documents;
    }
};

SearchServer create_search_server() {
    SearchServer search_server;
    search_server.set_stop_words(read_line());

    const int document_count = read_line_with_number();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.add_document(document_id, read_line());
    }

    return search_server;
}
int main() {
    const SearchServer search_server = create_search_server();
    //int document_id = 0;
    const string query = read_line();
    for (const auto& doc : search_server.find_top_documents(query)) {
        int document_id = doc.id;
        double relevance = doc.relevance;
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}
