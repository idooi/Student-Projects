#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cerr << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cerr << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define DEBUG_PRINT cerr << "DEBUG: "

#define EXEC(path, arg) \
  execvp((path), (arg));


void cleanBuffer(char* buf){
    for(int i=0; i <MAX_FILE_LINE;i++)
        buf[i] = EOF;
}

bool isInteger(const string & s)
{
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

    char * p;
    strtol(s.c_str(), &p, 10);

    return (*p == 0);
}

string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundCommand(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

/*void Command::CommandSetup(const char* cmd_line) {
    char **args = new char *[COMMAND_MAX_ARGS + 1];
    _parseCommandLine(cmd_line, args);
    commandName = params[0];
    string *params = new string[COMMAND_MAX_ARGS];
    for (int i = 0; i < COMMAND_MAX_ARGS; i++) {
        params[i] = args[i + 1];
    }
    delete[] args;//TODO:memory leak?
}
string interpretCommand(const char* cmd_line){
    char **args = new char *[COMMAND_MAX_ARGS + 1];
    _parseCommandLine(cmd_line, args);
    string commandName = args[0];
    delete[] args;//TODO: MEMORY LEAK
    return  commandName;
}*/

string Command::getRawCommand() {
    return rawCommand;
}

// TODO: Add your implementation for classes in Commands.h

SmallShell::SmallShell():
        prompt((string(DEFAULT_PROMPT)).c_str()), jobsList(new JobsList()),
        lastPwd(""), currentJob(nullptr){
    pid = getpid();
}

SmallShell::~SmallShell() {
// TODO: add your implementation
    delete jobsList;
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
    char **args = new char *[COMMAND_MAX_ARGS + 1];
    int num = _parseCommandLine(cmd_line, args);
    if (!args[0]) return nullptr;
    string rawCommandName = args[0];
    ///we want to remove backsign before parsing:
    char* copy_cmd = new char[rawCommandName.length() + 1];
    strcpy(copy_cmd, rawCommandName.c_str());
    _removeBackgroundSign(copy_cmd);
    string commandName = _trim(string(copy_cmd));
    delete [] copy_cmd;
    /*cout << commandName << endl;
    cout << (commandName == "showpid") << endl;*/
    for(int i = 0; i < num; i++){
        free(args[i]);
        args[i] = nullptr;
    }
    delete[] args;//TODO:memory leak?
    if ((int)string(cmd_line).find("|") != -1){
        return new PipeCommand(cmd_line);
    }
    if((int)string(cmd_line).find(">" ) != -1 ||
       (int)string(cmd_line).find(">>") != -1){//redirection
        return new RedirectionCommand(cmd_line);
    }
    if (commandName == "chprompt"){
        return new ChpromptCommand(cmd_line);
    }
    else if(commandName == "showpid") {
        return new ShowPidCommand(cmd_line);
    }
    else if(commandName == "pwd") {
        return new GetCurrDirCommand(cmd_line);
    }
    else if(commandName == "cd"){
        return new ChangeDirCommand(cmd_line, MY_SHELL.getlastPwd()) ;
    }
    else if(commandName == "jobs"){
        return new JobsCommand(cmd_line, MY_SHELL.getJobsList()) ;
    }
    else if(commandName == "kill"){
        return new KillCommand(cmd_line, MY_SHELL.getJobsList()) ;
    }
    else if(commandName == "fg"){
        return new ForegroundCommand(cmd_line,MY_SHELL.getJobsList());
    }
    else if(commandName == "bg"){
        return new BackgroundCommand(cmd_line,MY_SHELL.getJobsList());
    }
    else if(commandName == "quit"){
        return new QuitCommand(cmd_line, MY_SHELL.getJobsList());
    }
    else if(commandName == "cp"){
        return new CopyCommand(cmd_line);
    }
    else {//external
        return new ExternalCommand(cmd_line);
    }
    return nullptr;
}

Command::Command(const char *cmd_line) {
    setgrp = true;
    isBackground = _isBackgroundCommand(cmd_line);
    rawCommand = string(cmd_line);///copied with backsign
    char *args[COMMAND_MAX_ARGS + 1];
    for (int i = 0; i < COMMAND_MAX_ARGS + 1; i++){
        args[i] = nullptr;
    }

    ///we want to remove backsign before parsing:
    char* copy_cmd = new char[LINE_LEN];
    strcpy(copy_cmd,cmd_line);
    _removeBackgroundSign(copy_cmd);
    int num = _parseCommandLine(copy_cmd, args);///parsed args without backsign
    noBackCommand = string(copy_cmd);
    delete [] copy_cmd;

    numParams = 0 ;
    commandName = args[0];
    this->params = new string[COMMAND_MAX_ARGS];
    for (int i = 0; i < COMMAND_MAX_ARGS; i++) {
        if (args[i + 1]) numParams ++;
        params[i] = (!args[i + 1] ? "" : string(args[i + 1]))  ;
    }
    for(int i = 0; i < num; i++){
        free(args[i]);
        args[i] = nullptr;
    }
    closeChannel = -1;

}

Command::~Command() {
    delete[] this->params;
    params = nullptr;
}

ChangeDirCommand::ChangeDirCommand(const char *cmd_line, string plastPwd):
        BuiltInCommand(cmd_line){
    prevDir = plastPwd;
}

void ChpromptCommand::execute() {
    string newPrompt = (params[0] == "" ? DEFAULT_PROMPT : params[0]);
    MY_SHELL.setPrompt(newPrompt);
}

void ShowPidCommand::execute() {

    cout<<"smash pid is "<< MY_SHELL.getPid() << endl;
}

void GetCurrDirCommand::execute() {
    char buf[MAX_PATH_LEN];//TODO: set buffer size
    if(getcwd(buf,MAX_PATH_LEN)!= nullptr){
        cout << _trim(string(buf)) << endl;
    } else {
        //error caused by getcwd()
        echoSysError("getcwd failed");
    }
}

void ChangeDirCommand::execute() {
    char buf[MAX_PATH_LEN];
    if(getcwd(buf,MAX_PATH_LEN)== nullptr){
        //error caused by getcwd()
        echoSysError("getcwd failed");
        return;
    }
    if(!params[1].empty()){///More than one argument was sent
        echoError("cd", "too many arguments");
        return;
    }

    if(params[0].empty()){
        return;
    }

    if(params[0]=="-"){
        if(prevDir.empty()){
            echoError("cd", "OLDPWD not set");
            return;
        }
        if(chdir(prevDir.c_str())== -1) {
            echoSysError("chdir failed");
            return;
        }
        else{//success
            MY_SHELL.setLastPwd(_trim(string(buf)));
        }
        return;
    }

    if(chdir(params[0].c_str())== -1) {
        echoSysError("chdir failed");
        return;
    } else {//success/
        MY_SHELL.setLastPwd(_trim(string(buf)));
    }
}
void JobsCommand::execute() {
    jobs->removeFinishedJobs();
    jobs->printJobsList();
}

void KillCommand::execute() {

    string strJobId = params[1];
    string sigFlag  = params[0];
    if(!params[2].empty()){ //too many arguments
        echoError("kill", "invalid arguments");
        return;
    }
    if (strJobId.empty()  || !isInteger(strJobId)){//less than 2 args or none-integer jobid
        echoError("kill", "invalid arguments");
        return;
    }
    if ( /*sigFlag[0] != '-' ||*/ !isInteger(sigFlag)) {//signum is not an integer
        echoError("kill", "invalid arguments");
        return;
    }
    int jobId = atoi(strJobId.c_str());
    JobsList::JobEntry* job = jobsList->getJobById(jobId);
    if (!job){// job doesnt exist
        echoError("kill", "job-id " + to_string(jobId) + " does not exist");
        return;
    }

    if (sigFlag[0] != '-') {//signum doesnt have -
        echoError("kill", "invalid arguments");
        return;
    }
    int sigNum = atoi(sigFlag.substr(1).c_str());
    int pid = job->getPID();
    if (killpg(pid, sigNum) == -1){
        echoSysError("kill failed");
        return;
    }
    cout << "signal number " << sigNum << " was sent to pid " << pid << endl;
}

void ForegroundCommand::execute() {
    if (jobsList->isEmpty() && params[0].empty()){
        echoError("fg", "jobs list is empty");
        return;
    }

    if ((!isInteger(params[0]) && !params[0].empty())
        || !params[1].empty()){
        echoError("fg", "invalid arguments");
        return;
    }
    int jobId = atoi(params[0].c_str());
    JobsList::JobEntry* job = (params[0].empty() ?
                               jobsList->getLastJob() : jobsList->getJobById(jobId));
    if (!job){
        echoError("fg", "job-id " + to_string(jobId) + " does not exist");
        return;
    }
    job->print();
    int pid = job->getPID();
    if (killpg(pid, SIGCONT) == -1){
        echoSysError("kill failed");
    }
    job->setStopped(false);
    MY_SHELL.setCurrentJob(job);
    jobsList->removeJobById(job->getJID());
    int s = 0;
    waitpid(pid, &s, WUNTRACED);
}

void BackgroundCommand::execute() {
    if (jobsList->noStoppedJobs() && params[0].empty()){
        echoError("bg", "there is no stopped jobs to resume");
        return;
    }
    if ((!isInteger(params[0]) && !params[0].empty())
        ||!params[1].empty()){
        echoError("bg", "invalid arguments");
        return;
    }
    int jobId = atoi(params[0].c_str());
    JobsList::JobEntry* job = (params[0].empty() ?
                               jobsList->getLastStoppedJob() : jobsList->getJobById(jobId));
    if (!job){
        echoError("bg", "job-id " + to_string(jobId) +  " does not exist");
        return;
    }
    if (!job->getStopped()){
        echoError("bg", "job-id " + to_string(jobId) + " is already running in the background");
        return;
    }
    job->print();
    int pid = job->getPID();
    if (killpg(pid, SIGCONT) == -1){
        echoSysError("kill failed");
    }
    job->setStopped(false);
}

void QuitCommand::execute() {
    MY_SHELL.setCurrentJob(nullptr);
    if(params[0] == "kill"){
        int numOfJobs = jobsList->getNumOfJobs();
        cout << "smash: sending SIGKILL signal to " + to_string(numOfJobs) + " jobs:" << endl;
        jobsList->killAllJobs();
    }
    delete this;
    /*if (kill(getpid(), SIGKILL) == -1){
        echoSysError("kill failed");
    }*/
    exit(0);///to exit shell?
}

void CopyCommand::execute() {
    bool success = true;
    int pid = fork();
    if (pid == -1){
        echoSysError("fork failed");
        return;
    }
    if (pid == 0) {//child
        setpgrp();
        int backgroundPos = params[1].find("&");
        string fDestName = (backgroundPos != -1 ? params[1].substr(0, backgroundPos) : params[1]);
        string fSrcName  = params[0];
        //src validation:
        int fSrc;
        fSrc = open(params[0].c_str(), O_RDONLY);
        if (fSrc == -1) {
            echoSysError("open failed");
            exit(0);
        }
        close(fSrc);
        char * realSrc  = new char[MAX_PATH_LEN] ;
        char * realDest = new char[MAX_PATH_LEN] ;
        for (int i = 0; i < MAX_PATH_LEN; i++){
            realSrc [i] = '\0';
            realDest[i] = '\0';
        }
        realpath(fSrcName.c_str(),  realSrc);
        realpath(fDestName.c_str(), realDest);
        if (string(realSrc) == string(realDest)){ // same shit or src doesnt exist
            delete[] realSrc ;
            delete[] realDest;
            cout << "smash: " + params[0] + " was copied to " + params[1] << endl;
            exit(0);
        }
        // we actually need to do shit
        fSrc = open(params[0].c_str(), O_RDONLY);
        if (fSrc == -1) {
            echoSysError("open failed");
            exit(0);
        }

        int fDest = open(fDestName.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fDest == -1) {
            echoSysError("open failed");
            close(fSrc);
            exit(0);
        }
        char buf[MAX_FILE_LINE];
        //cleanBuffer(buf);
        int readResult = read(fSrc, buf, MAX_FILE_LINE);
        while (readResult > 0) {
            if (write(fDest, buf, readResult) == -1) {
                echoSysError("write failed");
                success = false;
                break;
            }
            // cleanBuffer(buf);
            readResult = read(fSrc, buf, MAX_FILE_LINE);
            if (readResult == -1) {
                echoSysError("read failed");
                success = false;
                break;
            }
        }
        if (close(fSrc) == -1) {
            echoSysError("close failed");
            success = false;
        }
        if (close(fDest) == -1) {
            echoSysError("close failed");
            success = false;
        }
        delete[] realSrc ;
        delete[] realDest;
        if (success){
            cout << "smash: " + params[0] + " was copied to " + params[1] << endl;
        }
        exit(0);
    }
    else { //father
        JobsList* jobsList = MY_SHELL.getJobsList();
        if (isBackground){
            jobsList->addJob(this, false, pid);
        }
        else{//foreground
            JobsList::JobEntry currJob = JobsList::JobEntry(DEF_JID,pid,
                                                            rawCommand,DEF_TIME,false);
            int s = 0;
            MY_SHELL.setCurrentJob(&currJob);
            waitpid(pid,&s,WUNTRACED);
            MY_SHELL.setCurrentJob(nullptr);
        }
    }


}

void ExternalCommand::execute() {
    char* exevArgs[4];
    exevArgs[0] = (char* const)"/bin/bash";
    exevArgs[1] = (char* const)"-c";
    exevArgs[2] = (char* const)noBackCommand.c_str();
    exevArgs[3] = nullptr;

    /**bash should get the command without the backsign, otherwise bash will
     do another fork*/
    JobsList* jobsList = MY_SHELL.getJobsList();
    if (jobsList->isFull()){
        return;//TODO:???
    }
    int pid = fork();
    if (pid == -1){
        echoSysError("fork failed");
        return;
    }
    if (pid == 0) {//child
        //  cerr << setgrp << endl;
        if (setgrp){
            setpgrp();
        }
        execv("/bin/bash", exevArgs);
    }
    else {//father
        if (isBackground){
            jobsList->addJob(this, false, pid);
        }
        else{//foreground
            if (setgrp) {
                JobsList::JobEntry currJob = JobsList::JobEntry(DEF_JID, pid,
                                                                rawCommand, DEF_TIME, false);
                MY_SHELL.setCurrentJob(&currJob);
            }
            int s = 0;
            if (closeChannel != -1){// for redirection
                // cerr << " im to close " << closeChannel << endl;
                close(closeChannel);
            }
            waitpid(pid,&s,WUNTRACED);
            MY_SHELL.setCurrentJob(nullptr);
        }
    }
}

void SmallShell::executeCommand(const char *cmd_line) {
    //cout<< "im in command"<<endl;
    MY_SHELL.getJobsList()->removeFinishedJobs();
    Command* cmd = CreateCommand(cmd_line);
    if (!cmd) return;
    cmd->execute();
    delete cmd;
    cmd = nullptr;
    // Please note that you must fork smash process for some commands (e.g., external commands....)
}

void Command::echo(string command, string msg) const {
    cout << "smash error: "<< command << ": " << msg << endl;
}

void Command::echoError(string command, string msg) const {
    cerr << "smash error: " + command + ": " + msg << endl;
}

void Command::echoSysError(string msg) const {
    perror(("smash error: " + msg).c_str());
}

/***************************START JOBS SECTION*********************************/
ostream& operator<<(ostream& os, const JobsList::JobEntry& job){
    string toPrint = "[" + to_string(job.JID) + "] ";
    toPrint += job.command + " : ";
    toPrint += to_string(job.PID) + " ";
    time_t currTime = time(nullptr); ///linux?
    int elapsed = (int)difftime(currTime, job.insertionTime);
    toPrint+= to_string(elapsed) + " secs";
    if(job.isStopped)
        toPrint += " (stopped)";
    os << toPrint;
    return os;
}

void JobsList::addJob(Command *cmd, bool isStopped, int pid) {
    int jobId = this->maxJId() + 1;
    string command = cmd->getRawCommand();
    time_t currTime = time(nullptr);///linux?
    JobsList::JobEntry newEntry = JobsList::JobEntry(jobId,pid,command,currTime,
                                                     isStopped);
    this->jobList.emplace_back(newEntry);
    sort(jobList.begin(),jobList.end());
}

void JobsList::addJob(JobsList::JobEntry *job) {
    MY_SHELL.getJobsList()->removeFinishedJobs();
    if(job->JID == DEF_JID){
        job->JID = this->maxJId() + 1;
    }
    job->insertionTime = time(nullptr);
    jobList.emplace_back(*job); ///by value?
    //cout<< "im going to sort"<< endl;
    sort(jobList.begin(),jobList.end());
}

void JobsList::printJobsList() {
    ///assuming we have already deleted all finished jobs and the list is sorted
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it!=this->jobList.end();++it){
        cout<<(*it)<<endl;
    }
}

void JobsList::removeFinishedJobs() {
    ///assuming the list is sorted
    int pid = 0 ;
    int status = 0 ;///needs to be sent to waitpid.
    for(vector<JobsList::JobEntry>::iterator it = jobList.begin();
        it != jobList.end();){
        pid = (*it).PID;
        ///Condition for a finished process:
        if(waitpid(pid, &status, WNOHANG) == pid) {
            jobList.erase(it);
        }
        else { ///job is not finished, update highest JID that we have seen.
            ++it;
        }
    }
    sort(jobList.begin(),jobList.end());
}
int JobsList::maxJId() {
    ///assuming the list is sorted
    int jid = 0 ;
    int maxJobId = 0;
    for(vector<JobsList::JobEntry>::iterator it = jobList.begin();
        it != jobList.end(); ++it){
        jid = (*it).JID;
        if (jid > maxJobId){
            maxJobId = jid;
        }
    }
    return maxJobId;
}

JobsList::JobEntry* JobsList::getJobById(int jobId) {
    ///assuming we have already deleted all finished jobs and the list is sorted
    JobsList::JobEntry* jobById = nullptr;
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it!=this->jobList.end();++it){
        if((*it).JID == jobId)
            jobById = &(*it);
    }
    return jobById;
}

void JobsList::removeJobById(int jobId) {
    ///assuming we have already deleted all finished jobs and the list is sorted
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it!=this->jobList.end();++it){

        if((*it).JID == jobId) {
            jobList.erase(it);
            break;
        }
    }
    sort(jobList.begin(),jobList.end());
}

bool JobsList::isEmpty() {
    return jobList.empty();
}

bool JobsList::isFull() {
    return jobList.size() == MAX_PROCESSES;
}


void JobsList::killAllJobs() {
    ///assuming we have already deleted all finished jobs and the list is sorted
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it != this->jobList.end();){
        int pid = (*it).PID;
        cout << to_string(pid) + ": " + (*it).command << endl;
        killpg(pid, SIGKILL);
        jobList.erase(it);
    }
}

JobsList::JobEntry* JobsList::getLastJob() {
    ///assuming we have already deleted all finished jobs and the list is sorted
    return &jobList.back();

}

JobsList::JobEntry* JobsList::getLastStoppedJob() {
    ///assuming we have already deleted all finished jobs and the list is sorted

    JobsList::JobEntry* lastStoppedJob = nullptr;
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it!=this->jobList.end();++it){
        if((*it).isStopped)
            lastStoppedJob = &(*it);
    }
    return lastStoppedJob;
}

int JobsList::getNumOfJobs() {
    return jobList.size();
}

void JobsList::JobEntry::print() {
    cout<<command + " : " + to_string(PID)<<endl;
}

bool JobsList::noStoppedJobs() {
    for(vector<JobsList::JobEntry>::iterator it = this->jobList.begin();
        it!=this->jobList.end();++it){
        if((*it).isStopped)
            return  false;
    }
    return true;
}
/*****************************END JOBS SECTION*********************************/
/*****************************START SPECIAL COMMANDS****************************/

void RedirectionCommand::execute() {
    bool append = false;
    append = ((int) rawCommand.find(">>") != -1);
    int redirectPos = rawCommand.find(">");
    string filename = (append ? noBackCommand.substr(redirectPos + 2) : noBackCommand.substr(redirectPos + 1));
    string command = rawCommand.substr(0, redirectPos);
    command = _trim(command);
    filename = _trim(filename);

    bool background = _isBackgroundCommand(rawCommand.c_str());
    if (_isBackgroundCommand(command.c_str())) {
        background = true;
        int backgroundPos = command.find("&");
        command = command.substr(0, backgroundPos);
        command = _trim(command);
    }

    Command *cmd = MY_SHELL.CreateCommand(command.c_str());
    if (!cmd) return;
    cmd->setSetgrp(false);
    JobsList *jobsList = MY_SHELL.getJobsList();
    /* No need to check processes overflow.
    if (jobsList->isFull()){
        return;//TODO:???
    }*/
    if (typeid(*cmd) != typeid(ExternalCommand)    &&
        typeid(*cmd) != typeid(CopyCommand)        &&
        typeid(*cmd) != typeid(RedirectionCommand) &&
        typeid(*cmd) != typeid(PipeCommand)){// dont fork for builtin
       // cout << "im nin not fork" << endl;
        int stdout_copy = dup(1);
        close(1);
        int fd;
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0666);
        if (fd == -1) {
            echoSysError("open failed");
            exit(0);
        }
        cmd->execute();
        close(fd);
        int s;
        JobsList::JobEntry * currJob = MY_SHELL.getCurrentJob();
        if (currJob){
            //cout << *currJob << endl;
            waitpid(currJob->getPID(), &s, WUNTRACED);
        }
        dup2(stdout_copy, 1);
        close(stdout_copy);
        return;
    }
    else {
        int pid = fork();
        if (pid == -1) {
            echoSysError("fork failed");
            return;
        }
        if (pid == 0) {//child:
            setpgrp();
            int stdout_copy = dup(1);
            close(1);
            int fd;
            fd = open(filename.c_str(), O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), 0666);
            if (fd == -1) {
                echoSysError("open failed");
                exit(0);
            }
            cmd->setCloseChannel(fd);
            cmd->execute();
          //  close(fd);
            dup2(stdout_copy, 1);
            close(stdout_copy);
            exit(0);

        } else {//father:
            if (background) {
                jobsList->addJob(this, false, pid);
            } else {//foreground
                JobsList::JobEntry currJob = JobsList::JobEntry(DEF_JID, pid,
                                                                rawCommand, DEF_TIME, false);
                int s = 0;
                MY_SHELL.setCurrentJob(&currJob);
                waitpid(pid, &s, WUNTRACED);
                MY_SHELL.setCurrentJob(nullptr);
            }
        }
    }
}

void PipeCommand::execute() {

    int pipePid = fork();
    if (pipePid == -1){
        echoSysError("fork failed");
        return;
    }
    if (pipePid == 0) {//pipe process
        setpgrp();
        string pipeSign = ((int) noBackCommand.find("|&") == -1 ? "|" : "|&");
        int outPutChannel = (pipeSign == "|" ? 1 : 2);
        string strCommand1 = noBackCommand.substr(0, noBackCommand.find(pipeSign));
        string strCommand2 = noBackCommand.substr(noBackCommand.find(pipeSign) + pipeSign.length());
        //cout << strCommand1 << " " << strCommand2 << endl;
        Command *cmd1 = MY_SHELL.CreateCommand(strCommand1.c_str());
        Command *cmd2 = MY_SHELL.CreateCommand(strCommand2.c_str());
        cmd1->setSetgrp(false);
        cmd2->setSetgrp(false);
        int fd[2];
        pipe(fd);
        int cmd1pid = fork();
        if (cmd1pid == -1) {
            echoSysError("fork failed");
            exit(0);
        }
        if (cmd1pid == 0) { //command 1
            //setpgrp();
            dup2(fd[1], outPutChannel);// |& or |
            //cerr << "d1: "<< d << endl;
            close(fd[0]);
            close(fd[1]);
            cmd1->execute();

            exit(0);
        }
        int cmd2pid = fork();
        if (cmd2pid == -1) {
            echoSysError("fork failed");
            exit(0);
        }
        if (cmd2pid == 0) {//command 2
            dup2(fd[0], 0);
            //cout << "d2: "<< d2 << endl;
            close(fd[0]);
            close(fd[1]);
            cmd2->execute();
            exit(0);
        }
        int s = 0;
        close(fd[0]);
        close(fd[1]);
        while (wait(&s) > 0);
        exit(0);
    }
    else {//shell
        JobsList* jobsList = MY_SHELL.getJobsList();
        if (isBackground){
            jobsList->addJob(this, false, pipePid);
        }
        else{//foreground
            JobsList::JobEntry currJob = JobsList::JobEntry(DEF_JID,pipePid,
                                                            rawCommand,DEF_TIME,false);
            int s = 0;
            MY_SHELL.setCurrentJob(&currJob);
            waitpid(pipePid, &s, WUNTRACED);
            MY_SHELL.setCurrentJob(nullptr);
        }
    }
}