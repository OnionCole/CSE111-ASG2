// $Id: file_sys.cpp,v 1.13 2022-01-26 16:10:48-08 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <iomanip>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   root = cwd = make_shared<inode> (file_type::DIRECTORY_TYPE);
   directory_entries& dirents = root->get_dirents();
   dirents.insert (dirent_type (".", root));
   dirents.insert (dirent_type ("..", root));
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
           << ", prompt = \"" << prompt() << "\""
           << ", file_type = " << root->contents->file_type());

   abs_path_str.push_back("/");
}

const string& inode_state::prompt() const { return prompt_; }

void inode_state::prompt (const string& new_prompt) {
   prompt_ = new_prompt;
}

inode_ptr inode_state::get_cwd() {
   return cwd;
}

void inode_state::fs_pwd() {
    // pwd

    for (auto iter = abs_path_str.begin();
           iter != abs_path_str.end(); ++iter) {
        cout << *iter;
        if (*iter != abs_path_str[0] && 
               iter != abs_path_str.end() - 1) {
            cout << "/";
        }
    }
    cout << endl;  // newline
}

void inode_state::fs_make(const wordvec& words) {
   // arg words: the words inputted to fn_make
   // make

   string fn = words.at(1);

   // create the file (if necessary) and get a ptr to its inode
   inode_ptr write_file;
   if (cwd->contents->file_exists(fn)) {  // the file 
         // already exists
      write_file = cwd->contents->get_dirents().at(fn);
   } else {
      write_file = cwd->contents->mkfile(fn);  // make new file
   }

   // write the data to the file
   write_file->contents->writefile({words.begin() + 2, words.end()});
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

directory_entries& inode::get_dirents() {
   return contents->get_dirents();
}

size_t inode::size() {
   // return the "size" of this inode, for a dir thats how many elements
         // for a file thats how many chars

   return contents->size();
}

void inode::fs_ls() {
   // ls with this inode as target (only words if inode points to a 
         // dir)

   contents->bf_ls();
}



file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + file_type());
}

directory_entries& base_file::get_dirents() {
   throw file_error ("is a " + file_type());
}

bool base_file::file_exists(const string&) {
   throw file_error("is a " + file_type());
}


void base_file::bf_ls() {
   throw file_error("is a " + file_type());
}



size_t plain_file::size() const {
   return 999;  // add up the total chars + spaces from the data field
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   // arg words: same as arg to fn_make, first 2 words are thus not
         // to be written
   // MEMLEAK FROM NOT FREEING THE OLD DATA?
   // CONSIDER: IS MAKING A NEW VECTOR OVER THE SAME SUB-ELEMENTS OK?

   DEBUGF ('i', words);

   // write data
   data = words;
}


size_t directory::size() const {
   return dirents.size();
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);

   inode_ptr new_inode = make_shared<inode>(file_type::PLAIN_TYPE);
   dirents.insert({filename, new_inode});

   return new_inode;
}

directory_entries& directory::get_dirents() {
   return dirents;
}

bool directory::file_exists(const string& name) {
   // check if a file name exists under this directory

   if (dirents.find(name) == dirents.end()) {  // does not exist
      return false;
   }
   return true;
}

void directory::bf_ls() {
   // do the ls output for this dir as the target

   map<string, inode_ptr>::iterator iter;
   for (iter = dirents.begin(); iter != dirents.end(); ++iter) {
      cout << iter->second->get_inode_nr() << std::setw(6);
      cout << iter->second->size() << std::setw(6);
      cout << iter->first << std::setw(6);
      cout << endl;
   }
}

