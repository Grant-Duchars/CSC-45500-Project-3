/**
 * Author: Grant Duchars
 */
#include <bits / stdc++.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "MemManager.hpp"
#include "Token.hpp"

using namespace std;

vector<string> split_at_semicolon(string str);

void MemManager::manage_memory() {
  Token tok = Token();
  auto commands = split_at_semicolon(_code);
  // Iterate through the verified code
  for (auto command : commands) {
    stringstream ss(command);
    tok.get(ss);
    auto first_id = tok.value();
    // By now the first id can only be free, compress, dump, or a variable
    if (first_id == "free") {
      // Get LPAREN
      tok.get(ss);
      // Get ID
      tok.get(ss);
      free(tok.value());
      continue;
    } else if (first_id == "compress") {
      compress();
      continue;
    } else if (first_id == "dump") {
      dump();
      continue;
    }
    // Get ASSIGNOP
    tok.get(ss);
    // Get ID
    tok.get(ss);
    auto second_id = tok.value();
    // Here second id can be either alloc or another variable
    if (second_id == "alloc") {
      // Get LPAREN
      tok.get(ss);
      // Get NUM_INT
      tok.get(ss);
      alloc(first_id, stoi(tok.value()));
      continue;
    }
    associate(second_id, first_id);
  }
}

void MemManager::alloc(string name, int size) {
  free(name);
  // Iterate through free list looking for first fit block of free space,
  // remove the found free block from the free list, allocate and add the
  // new variable to the variables map.
  for (auto it = _free_list.begin(); it != _free_list.end(); ++it) {
    auto block = *it;
    if (block.size >= size) {
      _free_list.erase(it);
      _variables.insert(
          {name, shared_ptr<Block>(new Block(block.address, size, 1))});
      // If the allocation didn't use the full free block create a
      // new free block for the free list whose size is the difference
      if (block.size - size != 0) {
        _free_list.push_back(Block(block.address + size, block.size - size, 0));
        sort(_free_list.begin(), _free_list.end());
      }
      return;
    }
  }
}

void MemManager::associate(string var_from, string var_to) {
  // Check if trying to associate with self
  if (var_from == var_to)
    return;
  // Find the current variable, increase the ref count, and add new
  // variable to map using the same block
  auto it = _variables.find(var_from);
  if (it != _variables.end()) {
    auto pair = *it;
    auto block = pair.second.get();
    block->references += 1;
    _variables.insert({var_to, shared_ptr<Block>(pair.second)});
  }
}

void MemManager::free(string var) {
  // Find the variable to free, decrement its ref count, and remove
  // it from the variables map. If its block's ref count is now zero
  // add the block back to the free list
  auto it = _variables.find(var);
  if (it != _variables.end()) {
    auto pair = *it;
    auto block = pair.second.get();
    block->references -= 1;
    _variables.erase(var);
    if (block->references == 0) {
      _free_list.push_back(Block(block->address, block->size, 0));
      sort(_free_list.begin(), _free_list.end());
    }
  }
}

void MemManager::compress() {
  auto it_a = _free_list.begin() + 1;
  auto it_b = _free_list.begin();
  while (it_a != _free_list.end()) {
    auto block_a = *it_a;
    auto block_b = *it_b;
    if (block_a.address == (block_b.address + block_b.size)) {
      *it_b = Block(block_b.address, block_b.size + block_a.size, 0);
      _free_list.erase(it_a);
      return compress();
    }
    it_a++;
    it_b++;
  }
}

void MemManager::dump() const {
  cout << "Variables:" << endl;
  for (const auto var : _variables) {
    cout << "  " << var.first << ": ";
    print_block(*var.second);
  }
  cout << "Free List:" << endl;
  for (const auto block : _free_list) {
    cout << "  ";
    print_block(block);
  }
  cout << "========================" << endl;
}

void MemManager::print_block(const Block &b) const {
  cout << b.address << " (" << b.size << ") [" << b.references << "]" << endl;
}

vector<string> split_at_semicolon(string str) {
  vector<string> vec;
  stringstream ss(str);
  string tok;
  while (getline(ss, tok, ';')) {
    vec.push_back(tok);
  }
  return vec;
}