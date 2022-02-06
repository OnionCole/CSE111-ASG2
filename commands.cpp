// $Id: commands.cpp,v 1.27 2022-01-28 18:11:56-08 - - $

#include "commands.h"
#include "debug.h"

const command_hash cmd_hash {
   {"#"     , fn_comment},
   {"cat"   , fn_cat    },
   {"cd"    , fn_cd     },
   {"echo"  , fn_echo   },
   {"exit"  , fn_exit   },
   {"ls"    , fn_ls     },
   {"lsr"   , fn_lsr    },
   {"make"  , fn_make   },
   {"mkdir" , fn_mkdir  },
   {"prompt", fn_prompt },
   {"pwd"   , fn_pwd    },
   {"rm"    , fn_rm     },
   {"rmr"   , fn_rmr    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result {cmd_hash.find (cmd)};
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such command");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status {exec::status()};
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}


void fn_comment(inode_state& state, const wordvec& words) {  // do nothing
   DEBUGF('c', state);
   DEBUGF('c', words);
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() == 1) {  // no args
      throw command_error("cat: no arg(s) given");
      return;
   }

   bool first_loop = true;
   for (auto iter = words.begin();
         iter != words.end(); ++iter) {
      if (first_loop) {
         first_loop = false;
         continue;
      }
      
      if ((*iter).back() == '/') {  // directory is given
         throw command_error("cat: cannot cat a directory");
         continue;
      }
      
      state.fs_cat(*iter);
   }
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() > 2) {
      throw command_error("cd: too many params");
      return;
   }
   if (words.size() == 1) {  // no args, so target is root
      state.fs_cd("/");
   } else {
      state.fs_cd(words.at(1));
   }
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() > 1) {  // exit code is given
      int status_val;
      try {
         status_val = stoi(words.at(1));
      } catch (...) {
         status_val = 127;
      }
      exec::status(status_val);
   }
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   DEBUGS ('l', 
      const auto& dirents = state.get_root()->get_dirents();
      for (const auto& entry: dirents) {
         cerr << "\"" << entry.first << "\"->" << entry.second << endl;
      }
    );

   if (words.size() == 1) {  // if no args are given, use cwd for a
         // single call
      state.get_cwd()->fs_ls();
   } else {  // we have to take each arg as a target

      //// find target for ls
      //inode_ptr target;

      //// do the ls
      //target->fs_ls();
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() == 1) {  // no args
      throw command_error("make: no arg(s) given");
      return;
   }
   if (words.at(1).back() == '/') {  // directory is given
      throw command_error("make: cannot make a directory");
      return;
   }

   state.fs_make(words);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   if (words.size() == 1) {  // no args
      throw command_error("mkdir: no arg(s) given");
      return;
   }

   state.fs_mkdir(words.at(1));
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string new_prompt = "";
   if (words.size() == 1) {
      new_prompt = " ";
   } else {
      bool first_loop = true;
      for (auto iter = words.begin();
            iter != words.end(); ++iter) {
         if (first_loop) {
            first_loop = false;
            continue;
         }
         new_prompt += *iter;
         new_prompt += " ";
      }
   }

   state.prompt(new_prompt);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   state.fs_pwd();
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

