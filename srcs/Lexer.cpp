#include "Lexer.hpp"

Lexer::Lexer(const char *filename) : filepath(filename) {
  directive_lookup["http"] = HTTP;
  directive_lookup["server"] = SERVERBLOCK;
  directive_lookup["keepalive_timeout"] = KEEPALIVE_TIMEOUT;
  directive_lookup["send_timeout"] = SEND_TIMEOUT;
  directive_lookup["listen"] = LISTEN;
  directive_lookup["server_name"] = SERVER_NAME;
  directive_lookup["root"] = ROOT;
  directive_lookup["autoindex"] = AUTOINDEX;
  directive_lookup["index"] = INDEX;
  directive_lookup["directory_listing"] = DIR_LISTING;
  directive_lookup["client_body_size"] = CLIENT_BODY_SIZE;
  directive_lookup["location"] = LOCATION;
  directive_lookup["methods"] = METHODS;
  directive_lookup["redirect"] = REDIRECT;
  directive_lookup["{"] = OPEN_CURLY_BRACKET;
  directive_lookup["}"] = CLOSED_CURLY_BRACKET;
  directive_lookup[";"] = SEMICOLON;

  readfileintobuffer();
  tokenize(buffer);
}

Lexer::~Lexer() {}

void Lexer::readfileintobuffer() {
  std::ifstream configFile(filepath.c_str());
  if (configFile.fail() == true) {
    std::cerr << "Error opening the file" << std::endl;
    return;
  }

  buffer.assign(std::istreambuf_iterator<char>(configFile),
                std::istreambuf_iterator<char>());

  configFile.close();
}

token Lexer::getTokenType(const std::string &type) {
  std::map<std::string, token>::iterator it = directive_lookup.find(type);
  if (it != directive_lookup.end())
    return it->second;
  return UNKNOWN;
}

static void trim(std::string &line) {
  size_t start = line.find_first_not_of(" \n\v\t\r\f");
  if (start == std::string::npos) {
    line = "";
    return;
  }

  size_t end = line.find_last_not_of(" \n\v\t\r\f") + 1;
  line = line.substr(start, end);
}

static bool checkHttpContext(std::string &line) {
  std::vector<std::string> words;
  std::istringstream ss(line);
  std::string word;

  while (ss >> word) {
    if (word[0] == '#')
      break;
    words.push_back(word);
  }

  if (words[0] != "http" && words[1] != "{")
    return false;
  return true;
}

void Lexer::createToken(std::vector<std::string>::iterator &begin, std::vector<std::string> &words, lexer_node &node){
	node.key = *begin;
	if (words.size() == 3 && ++begin != words.end())
		node.value = *begin;
	else {
		throw std::runtime_error("Syntax Error near: " + node.key);
	}
}

void Lexer::parseString(const std::string &line) {
  std::vector<std::string> words;
  std::istringstream ss(line);
  std::string directive;

  while (ss >> directive) {
    if (directive[0] == '#')
      break;
    if (directive.find(';') != std::string::npos) {
    	directive = directive.substr(0, directive.find_first_of(";"));
		if (words.size() != 2 && directive.empty() != true)
			words.push_back(directive);
    	words.push_back(";");
    } else {
      words.push_back(directive);
    }
  }

  std::vector<std::string>::iterator it;
  size_t counter = -1;
  for (it = words.begin(); it != words.end(); ++it) {
    lexer_node node;
    node.type = getTokenType(*it);

    switch (node.type) {
	case SERVERBLOCK:
    	node.value = "server";
    	break;
	case OPEN_CURLY_BRACKET:
    	node.value = "{";
    	break;
	case CLOSED_CURLY_BRACKET:
    	node.value = "}";
    	break;
	case SEMICOLON:
    	node.value = ";";
    	break;
    case KEEPALIVE_TIMEOUT:
    	createToken(it, words, node);
    	break;
	case SEND_TIMEOUT:
		createToken(it, words, node);
		break;
	case LISTEN:
		createToken(it, words, node);
		break;
	case SERVER_NAME:
		createToken(it, words, node);
		break;
	case ROOT:
		createToken(it, words, node);
		break;
	case AUTOINDEX:
		createToken(it, words, node);
		break;
	case INDEX:
		createToken(it, words, node);
		break;
	case DIR_LISTING:
		createToken(it, words, node);
		break;
	case CLIENT_BODY_SIZE:
		createToken(it, words, node);
		break;
	case LOCATION:
		createToken(it, words, node);
		break;
	case METHODS:
		node.key = *it;
		while (++it != words.end() && *it != ";") {
			node.value = node.value + " " + *it;
			counter++;
		}
		if (counter >= 3)
			throw std::runtime_error("Too many arguments: " + node.key);
		break;
	case REDIRECT:
		createToken(it, words, node);
		break;
    case UNKNOWN:
      throw std::runtime_error("Unkown token type " + *it);
    default:
      break;
    }
    lexer.push_back(node);
  }
}

void Lexer::tokenize(std::string &buffer) {
  std::istringstream ss(buffer);
  std::string line;

  while (std::getline(ss, line)) {
    trim(line);
    if (line.empty() == true || line[0] == '#')
      continue;
    break;
  }

  if (checkHttpContext(line) == false)
    throw std::runtime_error("Error in the config file: Expected 'http'");

  while (std::getline(ss, line)) {
    if (line.empty() == true)
      continue;
    trim(line);
    parseString(line);
  }
  lexer.pop_back();
}

std::vector<lexer_node> Lexer::getLexer() const { return this->lexer; }

std::ostream &operator<<(std::ostream &output, const Lexer &lexer) {
  std::vector<lexer_node>::iterator it;
  for (it = lexer.getLexer().begin(); it != lexer.getLexer().end(); it++) {
    output << "Type: " << it->type << ", Key: " << it->key
           << ", value: " << it->value << std::endl;
  }
  return output;
}
