
#include <iostream>
#include <signal.h>
#include "signals.h"
#include <sys/wait.h>
#include "Commands.h"
#include <unistd.h>


using namespace std;

void ctrlZHandler(int sig_num) {
    cout << "smash: got ctrl-Z"<< endl;
    JobsList::JobEntry* currJob =  MY_SHELL.getCurrentJob();
    if (!currJob) return;
    int pid = currJob->getPID();
    killpg(pid, SIGSTOP);
    currJob->setStopped(true);
    MY_SHELL.getJobsList()->addJob(currJob);
    cout << "smash: process " + to_string(currJob->getPID()) + " was stopped" << endl;
    MY_SHELL.setCurrentJob(nullptr);
}

void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C"<< endl;
    JobsList::JobEntry* currJob =  MY_SHELL.getCurrentJob();
    if (!currJob) return;
    int pid = currJob->getPID();
    killpg(pid, SIGKILL);
    cout << "smash: process " + to_string(currJob->getPID()) + " was killed"  << endl;
    MY_SHELL.setCurrentJob(nullptr);
}

void alarmHandler(int sig_num) {
    cout << "smash got an alarm" << endl;

}