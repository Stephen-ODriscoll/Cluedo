#pragma once

#define MIN_PLAYERS 2
#define MAX_PLAYERS 6
#define NUM_CATEGORIES 3

#define CRITICAL(title, desc)   QMessageBox msgBox; \
                                msgBox.critical(0, title, desc);

#define TRY     try \
                {

#define CATCH   } \
                catch (const contradiction& ex) \
                { \
                    CRITICAL("Contraditory Info Given", ex.what()) \
                } \
                catch (const std::exception& ex) \
                { \
                    CRITICAL("Exception Occured", ex.what()) \
                }
