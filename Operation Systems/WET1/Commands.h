#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_
#include <string.h>
#include <vector>
#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)
#define MAX_PATH_LEN (100)
#define MAX_PROCESSES (100)
#define LINE_LEN (80)
#define DEFAULT_PROMPT "smash"
#define MY_SHELL SmallShell::getInstance()
#define DEF_JID (-1) //for new foreground process
#define DEF_TIME (0) //for new foreground process
#define MAX_FILE_LINE 240


using namespace std;

class Command {
public:
// TODO: Add your data members
protected:
    string commandName;
    string* params;
    string rawCommand;
    string noBackCommand;
    bool isBackground;
    int numParams;
    bool setgrp;
    int  closeChannel;
public:
    Command(const char* cmd_line);
    void CommandSetup(const char* cmd_line);
    virtual ~Command();
    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
    void echo (string command, string msg) const;
    void echoSysError (string msg) const;
    void echoError (string command, string msg) const;
    string getRawCommand();
    void setSetgrp(bool set){ setgrp = set;};
    void setCloseChannel(int channel){
        closeChannel = channel;
    }
};

class BuiltInCommand : public Command {
    ///BuiltInCommands should ignore & sign
public:
    BuiltInCommand(const char* cmd_line):Command(cmd_line){}
    virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line):Command(cmd_line){}
    virtual ~ExternalCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char* cmd_line): Command(cmd_line){};
    virtual ~PipeCommand() {}
    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char* cmd_line):
            Command(cmd_line)
    {};
    virtual ~RedirectionCommand() {}
    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
    string prevDir;
public:
    ChangeDirCommand(const char* cmd_line, string plastPwd);//dont kill me
    virtual ~ChangeDirCommand() {}
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){}
    virtual ~GetCurrDirCommand() {}
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line):BuiltInCommand(cmd_line){};
    virtual ~ShowPidCommand() {}
    void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
    JobsList * jobsList;
public:
    QuitCommand(const char* cmd_line, JobsList* jobs):
            BuiltInCommand(cmd_line), jobsList(jobs){};
    virtual ~QuitCommand() {}
    void execute() override;
};
class JobsList {
public:
    class JobEntry {
        // TODO: Add your data members
        int JID;
        int PID;
        string command;
        time_t insertionTime;
        bool isStopped;
    public:
        JobEntry() = default;
        JobEntry(int JID, int PID, string command,  time_t time, bool
        isStopped):
                JID(JID), PID(PID), command(command), insertionTime(time),
                isStopped (isStopped){};

        JobEntry(const JobEntry& other) = default;

        JobEntry& operator = (const JobEntry& other) = default;
        bool operator < (const JobEntry& other){
            return this->JID < other.JID;
        }

        friend ostream& operator<<(ostream& os, const JobEntry& job);
        friend class JobsList;

        int getJID(){ return JID;}
        int getPID(){ return PID;}
        bool getStopped(){ return isStopped;}
        void setStopped(bool stopped) {isStopped=stopped;}
        void setJID    (int newJID)   {JID = newJID;}
        void print();

    };
// TODO: Add your data members
    vector<JobEntry> jobList;
    //int maxId;
public:
    JobsList()/*:maxId(0) */{};
    ~JobsList() = default;
//void addJob(Command* cmd, bool isStopped = false);
    void addJob(Command* cmd, bool isStopped, int pid);
    void addJob(JobEntry* job);
    void printJobsList();
    void killAllJobs();//kill all
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    void removeJobById(int jobId);
    JobEntry * getLastJob();
    JobEntry *getLastStoppedJob();
    bool isFull();
    bool noStoppedJobs();
    bool isEmpty();
    int getNumOfJobs();
    int maxJId();
// TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {// print all jobs
    // TODO: Add your data members
    JobsList * jobs;
public:
    JobsCommand(const char* cmd_line, JobsList* jobs):
            BuiltInCommand(cmd_line), jobs(jobs){};
    virtual ~JobsCommand() {}
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
    JobsList* jobsList;
public:
    KillCommand(const char* cmd_line, JobsList* jobs):
            BuiltInCommand(cmd_line), jobsList(jobs){};
    virtual ~KillCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    JobsList* jobsList;
    // TODO: Add your data members
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs):
            BuiltInCommand(cmd_line), jobsList(jobs){};
    virtual ~ForegroundCommand() {}
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    JobsList* jobsList;
    // TODO: Add your data members
public:
    BackgroundCommand(const char* cmd_line, JobsList* jobs):
            BuiltInCommand(cmd_line), jobsList(jobs){};
    virtual ~BackgroundCommand() {}
    void execute() override;
};


// TODO: should it really inherit from BuiltInCommand ?
class CopyCommand : public Command{
public:
    CopyCommand(const char* cmd_line):
            Command(cmd_line){};
    virtual ~CopyCommand() {}
    void execute() override;
};

// TODO: add more classes if needed
// maybe chprompt , timeout ?
class ChpromptCommand: public BuiltInCommand {
public:
    ChpromptCommand(const char* cmd_line):BuiltInCommand(cmd_line){};
    virtual ~ChpromptCommand() {}
    void execute() override;
};

class SmallShell {
private:
    string      prompt;
    int            pid;
    JobsList* jobsList;
    string     lastPwd;///for cd command
    //string     secondLastPwd;
    JobsList::JobEntry* currentJob;
    // TODO: Add your data members
    SmallShell();
public:
    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    int   getPid(){ return pid;};
    string getPrompt()     const { return prompt  ; };
    string getlastPwd()    const { return lastPwd ; };


    JobsList*  getJobsList() const { return jobsList; };
    JobsList::JobEntry*  getCurrentJob() const { return currentJob; };
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    // TODO: add extra methods as needed
    void setPrompt(string newPrompt){
        prompt = newPrompt;
    }
    void setCurrentJob(JobsList::JobEntry* newJob) {
        delete currentJob;
        currentJob = nullptr;
        if (newJob) {
            currentJob = new JobsList::JobEntry(*newJob);
        }
    }
    void setLastPwd(string newLastPwd){
       lastPwd = newLastPwd;
    };

   /* void swapLastPwds(){
        string temp = lastPwd;
        lastPwd = secondLastPwd;
        secondLastPwd = temp;
    }
    void updateLastPwds(string newPwd){
        secondLastPwd = lastPwd;
        lastPwd = newPwd;
    }*/
};

#endif //SMASH_COMMAND_H_