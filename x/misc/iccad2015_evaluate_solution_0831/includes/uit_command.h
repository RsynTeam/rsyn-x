/******************************************************************************
 *                                                                            *
 * Copyright (c) 2015, Tsung-Wei Huang and Martin D. F. Wong,                 *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef UIT_COMMAND_H_
#define UIT_COMMAND_H_

#include "uit_typedef.h"

namespace uit {

// Class: Command
class Command {
  
  public:

    Command();                                              // Constructor.
    ~Command();                                             // Destructor.

    void_t init_iccad2015_command(int argc, char *argv[]);  // Set the command from ICCAD15 contest.
    void_t init_tau2015_command(int argc, char *argv[]);    // Set the command from TAU2015 contest.

    inline string_crt tau2015_wrapper_fpath() const;        // Query the tau2015_wrapper file path.
    inline string_crt tau2015_wrapper_fdir() const;         // Query the tau2015_wrapper file dir.
    inline string_crt timing_fpath() const;                 // Query the timing file path.
    inline string_crt ops_fpath() const;                    // Query the ops file path.
    inline string_crt output_fpath() const;                 // Query the output file path.
    inline string_crt early_celllib_fpath() const;          // Query th early library file path.
    inline string_crt late_celllib_fpath() const;           // Query the late library file path.
    inline string_crt spef_fpath() const;                   // Query the spef file path.
    inline string_crt verilog_fpath() const;                // Query the verilog file path.

  private:

    string_1D_vt _argv;                                     // Argument vector.

    string_t _tau2015_wrapper_fpath;                        // Wrapper file path.
    string_t _tau2015_wrapper_fdir;                         // Wrapper file dir.
    string_t _timing_fpath;                                 // Timing file path.
    string_t _ops_fpath;                                    // Operation file path.
    string_t _output_fpath;                                 // Output file path.
    string_t _early_celllib_fpath;                          // Early celllib file path.
    string_t _late_celllib_fpath;                           // Late celllib file path.
    string_t _spef_fpath;                                   // Spef file path.
    string_t _verilog_fpath;                                // Verilog file path.
};

// Function: tau2015_wrapper_fpath
// Return the file path of the warpper.
inline string_crt Command::tau2015_wrapper_fpath() const {
  return _tau2015_wrapper_fpath;
}

// Function: tau2015_wrapper_fdir
// Return the file path of the warpper.
inline string_crt Command::tau2015_wrapper_fdir() const {
  return _tau2015_wrapper_fdir;
}

// Function: timing_fpath
// Return the timing file path.
inline string_crt Command::timing_fpath() const {
  return _timing_fpath;
}

// Function: ops_fpath
// Return the operation file path.
inline string_crt Command::ops_fpath() const {
  return _ops_fpath;
}

// Function: output_fpath
// Return the output file path.
inline string_crt Command::output_fpath() const {
  return _output_fpath;
}

// Function: early_celllib_fpath
// Return the early celllib file path.
inline string_crt Command::early_celllib_fpath() const {
  return _early_celllib_fpath;
}

// Function: late_celllib_fpath
// Return the late celllib file path.
inline string_crt Command::late_celllib_fpath() const {
  return _late_celllib_fpath;
}

// Function: spef_fpath
// Return the spef file path.
inline string_crt Command::spef_fpath() const {
  return _spef_fpath;
}

// Function: verilog_fpath
// Return the verilog file path.
inline string_crt Command::verilog_fpath() const {
  return _verilog_fpath;
}

};

#endif

