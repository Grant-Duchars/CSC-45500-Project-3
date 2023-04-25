/**
 * Author: Grant Duchars
 */
#ifndef _MEMMANAGER_HPP_
#define _MEMMANAGER_HPP_

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class MemManager {
private:
  // Private Structs
  class Block {
  public:
    int address;
    int size;
    int references;
    Block(int a, int s, int r) : address(a), size(s), references(r) {}
    bool operator<(Block const &other) { return address < other.address; }
  };

  // Private Variables
  int _free_list_size;
  std::string _code;
  std::vector<Block> _free_list;
  std::unordered_map<std::string, std::shared_ptr<Block>> _variables;

  // Private Methods
  void alloc(std::string name, int quantity);
  void associate(std::string var_from, std::string var_to);
  void free(std::string var);
  void compress();
  void dump() const;
  void print_block(const Block &) const;

public:
  MemManager(int fls, std::string code)
      : _free_list_size(fls), _code(code), _free_list(), _variables() {
    _free_list.push_back(Block(0, fls, 0));
  }

  void manage_memory();
};
#endif