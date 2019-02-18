/*--------------------------------------------------------------------------*
 *----									----*
 *----		mamaKangaroo.c						----*
 *----									----*
 *----	    This program simulates a mama kangaroo waiting for her	----*
 *----	joeys using UNIX signaling.					----*
 *----									----*
 *----	----	----	----	----	----	----	----	----	----*
 *----									----*
 *----	Version 1.0		2017 April 17		Joseph Phillips	----*
 *----									----*
 *--------------------------------------------------------------------------*/

//
//		Header inclusion:
//
#include "kangarooHeaders.h"

//
//		Definition of global vars:
//

//  PURPOSE:  To hold the random number seed the user requested.
int randomNumSeed;

//  PURPOSE:  To hold the number of joeys the user requested to make.
int numJoeys = MIN_NUM_LEGAL_JOEYS - 1;

//  PURPOSE:  To hold the number of joeys that have not yet found their way
//	back to their mama's pouch.
int numActiveJoeys = 0;

//  PURPOSE:  To hold the process ids of the joey processes.
pid_t *joeyPidArray = NULL;

//  PURPOSE:  To hold the process id of the mall process.
pid_t mallPid = -1;

//
//		Definition of global fncs:
//

//  PURPOSE:  To initialize 'numJoeys' from the 'argc' commamnd line
//	parameters give in 'argv[]'.  No return value.
void initializeNumJoeys(int argc,
                        char *argv[])
{
  //  I.  Application validity check:

  //  II.  Initialize 'numJoeys':
  char line[LINE_LEN];

  if (argc > MAMAS_NUM_JOEYS_CMD_LINE_INDEX)
    numJoeys = (int)strtol(argv[MAMAS_NUM_JOEYS_CMD_LINE_INDEX], NULL, 10); // <-- REPLACE with code that gets integer from 'argv[MAMAS_NUM_JOEYS_CMD_LINE_INDEX]'

  while (numJoeys < MIN_NUM_LEGAL_JOEYS)
  {
    printf("Please enter the number of joeys that"
           " get lost in the shopping mall: ");
    fgets(line, LINE_LEN, stdin);
    numJoeys = (int)strtol(line, NULL, 10);
    //numJoeys	= 0; // <-- REPLACE with code that gets integer from 'line'
  }

  //  III.  Finished:
}

//  PURPOSE:  To initialize 'randomNumSeed' from the 'argc' commamnd line
//	parameters give in 'argv[]'.  No return value.
void initializeRandomNumSeed(int argc,
                             char *argv[])
{
  //  I.  Application validity check:

  //  II.  Initialize 'randomNumSeed':
  char line[LINE_LEN];

  if (argc > MAMAS_RAND_SEED_CMD_LINE_INDEX)
    randomNumSeed = (int)strtol(argv[MAMAS_RAND_SEED_CMD_LINE_INDEX], NULL, 10);
  //randomNumSeed	= 0; // <-- REPLACE with code that gets integer from 'argv[MAMAS_RAND_SEED_CMD_LINE_INDEX]'
  else
  {
    printf("Please enter the random number seed: ");
    fgets(line, LINE_LEN, stdin);
    randomNumSeed = (int)strtol(line, NULL, 10);
    //randomNumSeed	= 0; // <-- REPLACE with code that gets integer from 'line'
  }

  //  III.  Finished:
}

//  PURPOSE:  To handle signal 'SIGCHLD'.  'sig' is ignored because it is known
//	to be 'SIGCHLD'.  No return value.
void sigChldHandler(int sig)
{
  //  I.  Application validity check:

  //  II.  Search for the joey with process id equal to 'childPid':
  int status;
  pid_t childPid;
  int joey;
  int i = 0;

  while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
  {
    if ((WIFEXITED(status)) && (WEXITSTATUS(status) == EXIT_SUCCESS))
    {
      for (i = 0; i < numJoeys; i++)
      {
        if (joeyPidArray[i] == childPid)
        {
          printf("Mama: Joey # %d complete.\n", i);
          numActiveJoeys--;
          break;
        }
      }
    }
  }

  //*

  //  YOUR CODE HERE that uses a 'while()' loop testing 'waitpid()' to
  //   get *all* finished children.  Each should be tested against
  //   'joeyPidArray[joey]' to see if it is that particular joey.
  //  Decrements 'numActiveJoeys' for each stopped joey.

  //  III.  Finished:
}

//  PURPOSE:  To install 'sigChldHandler()' as the SIGCHLD handler, which is
//	able to distinguish among which of the children has finished.  No
//	parameters.  No return value.
void installSigChldHandler()
{
  //  I.  Application validity check:

  struct sigaction signal;

  memset(&signal, '\0', sizeof(struct sigaction));
  signal.sa_handler = sigChldHandler;
  signal.sa_flags = SA_NOCLDSTOP | SA_RESTART;
  sigaction(SIGCHLD, &signal, NULL);

  //  II.  Install the handler:
  //  YOUR CODE HERE

  //  III.  Finished:
}

//  PURPOSE:  To start the mall process.  No parameters.  No return value.
void startMallProcess()
{
  //  I.  Application validity check:

  //  II.  Start the mall process:
  mallPid = fork();

  //mallPid	= 0; // <-- REPLACE with code that returns pid of new process

  if (mallPid == -1)
  {
    fprintf(stderr, "Can't make any new processes! :(\n");
    exit(EXIT_FAILURE);
  }

  if (mallPid == 0)
  {
    char numJoeysStr[LINE_LEN];
    char randNumSeedStr[LINE_LEN];

    snprintf(numJoeysStr, LINE_LEN, "%d", numJoeys);
    snprintf(randNumSeedStr, LINE_LEN, "%d", randomNumSeed);

    execl(MALL_PROG_NAME, MALL_PROG_NAME, numJoeysStr, randNumSeedStr, NULL);

    //  YOUR CODE HERE that runs 'MALL_PROG_NAME' with command line parameters 'numJoeysStr' and 'randNumSeedStr'.
    fprintf(stderr, "Could not find the mall! :(\n");
    exit(EXIT_FAILURE);
  }

  //  III.  Finished:
}

//  PURPOSE:  To start the joey processes.  No parameters.  No return value.
void startJoeyProcesses()
{
  //  I.  Application validity check:

  //  II.  Start the joey processes:
  int joey;
  joeyPidArray = (pid_t *)calloc(sizeof(pid_t), numJoeys);

  for (joey = 0; joey < numJoeys; joey++)
  {
    joeyPidArray[joey] = fork();
    //joeyPidArray[joey]	= 0; // <-- REPLACE with code that returns pid of new process

    if (joeyPidArray[joey] == -1)
    {
      fprintf(stderr, "Can't make any new processes! :(\n");
      exit(EXIT_FAILURE);
    }

    if (joeyPidArray[joey] == 0)
    {
      char indexStr[LINE_LEN];
      char mallPidStr[LINE_LEN];
      char randNumSeedStr[LINE_LEN];

      snprintf(indexStr, LINE_LEN, "%d", joey);
      snprintf(mallPidStr, LINE_LEN, "%d", mallPid);
      snprintf(randNumSeedStr, LINE_LEN, "%d", randomNumSeed + joey);

      execl(JOEY_PROG_NAME, JOEY_PROG_NAME, indexStr, mallPidStr, randNumSeedStr, NULL);

      //  YOUR CODE HERE that runs 'JOEY_PROG_NAME' with command line parameters 'indexStr', 'mallPidStr' and 'randNumSeedStr'.
      fprintf(stderr, "Could not find the joey! :(\n");
      exit(EXIT_FAILURE);
    }

    numActiveJoeys++;
  }

  //  III.  Finished:
}

int main(int argc,
         char *argv[])
{
  //  I.  Application validity check:

  //  II.  Do simulation:

  //  II.A.  Get simulation parameters:
  initializeNumJoeys(argc, argv);
  initializeRandomNumSeed(argc, argv);

  //  II.B.  Setup handlers:
  installSigChldHandler();

  //  II.C.  Initialize processes:
  startMallProcess();
  sleep(1);
  startJoeyProcesses();

  //  II.D.  To do the simulation by waiting until all joeys have returned:
  while (numActiveJoeys > 0)
    sleep(1);

  //  II.E.  Clean up after game:
  //  II.E.1.  Stop the mall process:
  // YOUR CODE HERE to send 'SIGINT' to 'mallPid'

  //  II.E.2.  Release mem:
  free(joeyPidArray);

  //  III.  Finished:
  return (EXIT_SUCCESS);
}