/******************************************************************************
*******************************************************************************
*******************************************************************************

    ferris fchmod
    Copyright (C) 2010 Ben Martin

    libferris is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libferris is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libferris.  If not, see <http://www.gnu.org/licenses/>.

    For more details see the COPYING file in the root directory of this
    distribution.

    $Id: ls.cpp,v 1.12 2008/04/27 21:30:11 ben Exp $

*******************************************************************************
*******************************************************************************
******************************************************************************/

#include <config.h>

#include <string>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <Ferris/Ferris.hh>
#include <Ferrisls.hh>
#include <Ferris/Ferrisls_AggregateData.hh>
#include <Ferris/FerrisBoost.hh>

#include <popt.h>
#include <unistd.h>

using namespace std;
using namespace Ferris;

#define DEBUG 0

const string PROGRAM_NAME = "fchmod";

void usage(poptContext optCon, int exitcode, char *error, char *addl)
{
    
    poptPrintUsage(optCon, stderr, 0);
    if (error) fprintf(stderr, "%s: %s0", error, addl);
    exit(exitcode);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int hadErrors = 0;

unsigned long ChangesVerbose = 0;
unsigned long IgnoreErrors = 0;
unsigned long PreserveRoot = 0;
unsigned long DoNotPreserveRoot = 0;
const char* ReferenceFile = 0;
const char* SilentCompleteRegex = 0;
unsigned long Verbose = 0;
unsigned long ShowVersion = 0;
unsigned long Recurse = 0;

string targetMode;

void visit( fh_context c )
{
   bool dontDelegateToOvermountContext = true;
   bool throw_for_errors = !IgnoreErrors;
   bool create = true;
   bool PerformChange = true;

   if( PerformChange )
   {
      if( !targetMode.empty() )
         setStrAttr( c, "mode", targetMode,
                     create, throw_for_errors,  
                     dontDelegateToOvermountContext );
   }
   
   if( Recurse && c->isDir() )
   {
      if( isTrue( getStrAttr( c, "dontfollow-is-link", "0" )))
      {
      }
      else
      {
         Context::iterator ci = c->begin();
         for( ; ci != c->end(); ++ci )
         {
            visit( *ci );
         }
      }
   }
   if( ChangesVerbose )
   {
      cout << "changed mode of `" << c->getURL() << "' to "
           << targetMode << endl;
   }
}

int main( int argc, const char** argv )
{
    struct poptOption optionsTable[] = {

       { "changes", 'c', POPT_ARG_NONE, &ChangesVerbose, 0,
         "Verbosely describe the action for each FILE whose ownership "
         "actually changes.", 0 },

       { "quiet", 'f', POPT_ARG_NONE, &IgnoreErrors, 0,
         "Do not print error messages about files whose ownership cannot be changed.", 0 },

       { "silent", 0, POPT_ARG_NONE, &IgnoreErrors, 0,
         "Do not print error messages about files whose ownership cannot be changed.", 0 },

       { "preserve-root", 0, POPT_ARG_NONE, &PreserveRoot, 0,
         "Fail upon any attempt to recursively change the root directory", 0 },

       { "no-preserve-root", 0, POPT_ARG_NONE, &DoNotPreserveRoot, 0,
         "Cancel the effect of any preceding `--preserve-root' option.", 0 },

       { "reference", 0, POPT_ARG_STRING, &ReferenceFile, 0,
         "Change the user and group of each FILE to be the same as those of REF_FILE", 0 },

       { "silent-complete-for-regex", 0, POPT_ARG_STRING, &SilentCompleteRegex, 0,
         "if the URL matches the regex then silently return OK.", 0 },
       
       { "verbose", 'v', POPT_ARG_NONE, &Verbose, 0,
         "Output a diagnostic for every file processed", 0 },

       { "version", 0, POPT_ARG_NONE, &ShowVersion, 0,
         "Show version information and quit", 0 },
       
       { "recursive", 'R', POPT_ARG_NONE, &Recurse, 0,
         "Recursively change ownership of directories and their contents", 0 },

        /*
         * Standard Ferris options
         */
        FERRIS_POPT_OPTIONS

        /**
         * Expansion of strange-url://foo*
         */
        FERRIS_SHELL_GLOB_POPT_OPTIONS
        POPT_AUTOHELP
        POPT_TABLEEND
   };
    poptContext optCon;


    optCon = poptGetContext(PROGRAM_NAME.c_str(), argc, argv, optionsTable, 0);
    poptSetOtherOptionHelp(optCon, "fchmod [OPTION]... {MODE | --reference=REF_URL} URL1 URL2 ...");
                                                        
    if (argc < 1) {
//        poptPrintHelp(optCon, stderr, 0);
        poptPrintUsage(optCon, stderr, 0);
        exit(1);
    }

    
    /* Now do options processing, get portname */
    int c=-1;
    while ((c = poptGetNextOpt(optCon)) >= 0)
    {
//         switch (c) {
//         }
    }

    if( ShowVersion )
    {
       cout << PROGRAM_NAME << " version: $Id: ls.cpp,v 1.12 2008/04/27 21:30:11 ben Exp $\n"
            << "ferris   version: " << VERSION << nl
            << "Written by Ben Martin, aka monkeyiq" << nl
            << nl
            << "Copyright (C) 2001-2010 Ben Martin" << nl
            << "This is free software; see the source for copying conditions.  There is NO\n"
            << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
            << endl;
       exit(0);
    }

    if( PreserveRoot || DoNotPreserveRoot )
       cerr << "WARNING: These features are not implemented yet!" << endl;
    
    try
    {
        stringlist_t srcs;
        srcs = expandShellGlobs( srcs, optCon );
        if( srcs.empty() )
        {
           poptPrintUsage(optCon, stderr, 0);
           exit(1);
        }
        stringlist_t::iterator srcsiter = srcs.begin();
        stringlist_t::iterator srcsend  = srcs.end();
        
        if( !ReferenceFile )
        {
           ::mode_t m = Factory::MakeInitializationMode( *srcsiter );
           {
              stringstream ss;
              ss << oct << m;
              targetMode = ss.str();
           }
//           cerr << "targetMode:" << targetMode << endl;
           ++srcsiter;
        }
        if( ReferenceFile )
        {
           fh_context c = Resolve( ReferenceFile );
           targetMode   = getStrAttr( c, "mode", "" );
        }
        
        bool Done = false;
        for( int  First_Time = 1; !Done && srcsiter != srcsend ; ++srcsiter )
        {
            string earl = *srcsiter;
            First_Time = 0;

            if( SilentCompleteRegex &&
                regex_search( earl, toregex(SilentCompleteRegex) ))
            {
               continue;
            }
            
            fh_context c = Resolve( earl );
            visit( c );
        }
    }
    catch( NoSuchContextClass& e )
    {
//        cerr << "Invalid context class given:" << RootContextClass << endl;
        cerr << "e:" << e.what() << endl;
        exit(1);
    }
    catch( exception& e )
    {
        cerr << "ls.cpp cought:" << e.what() << endl;
        exit(1);
    }

    poptFreeContext(optCon);
    cout << flush;
    return hadErrors;
}
