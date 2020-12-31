#pragma once

#define MIN_PLAYERS 2
#define MAX_PLAYERS 6
#define NUM_CATEGORIES 3

#define TRY     try \
                {

#define CATCH   } \
                catch (const contradiction& ex) \
                { \
                    QMessageBox msgBox; \
                    msgBox.critical(0, "Contraditory Info Given", ex.what()); \
                } \
                catch (const std::exception& ex) \
                { \
                    QMessageBox msgBox; \
                    msgBox.critical(0, "Exception Occured", ex.what()); \
                }