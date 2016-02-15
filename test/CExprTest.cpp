#include <CFile.h>
#include <CStrUtil.h>
#include <CExpr.h>
#include <CReadLine.h>
#include <CParseLine.h>
#include <cstdio>
#include <cstdlib>

#define FUNCTION_PARSE   1
#define FUNCTION_INTERP  2
#define FUNCTION_COMPILE 3
#define FUNCTION_EXECUTE 4

static int function = FUNCTION_EXECUTE;

static void processFile(const std::string &filename);
static void mainLoop();
static bool processLine(const std::string &line);

CExpr *expr;

//---

extern int
main(int argc, char **argv)
{
  bool debug = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'c')
        function = FUNCTION_COMPILE;
      else if (argv[i][1] == 'e')
        function = FUNCTION_EXECUTE;
      else if (argv[i][1] == 'i')
        function = FUNCTION_INTERP;
      else if (argv[i][1] == 'p')
        function = FUNCTION_PARSE;
      else if (argv[i][1] == 'd')
        debug = true;
    }
    else
      files.push_back(argv[i]);
  }

  expr = new CExpr;

  expr->setDebug(debug);

  uint num_files = files.size();

  if (num_files > 0) {
    for (uint i = 0; i < num_files; i++)
      processFile(files[i]);
  }
  else
    mainLoop();

  exit(0);
}

static void
processFile(const std::string &filename)
{
  CFile file(filename);

  expr->createRealVariable("x", 1.0);

  std::vector<std::string> lines;

  file.toLines(lines);

  for (uint i = 0; i < lines.size(); i++)
    (void) processLine(lines[i]);
}

static void
mainLoop()
{
  CReadLine readline;

  for (;;) {
    std::string line = readline.readLine();

    std::string line1 = CStrUtil::stripSpaces(line);
    if (line1 == "exit") break;

    if      (line1 == "degrees")
      expr->setDegrees(true);
    else if (line1 == "radians")
      expr->setDegrees(false);
    else
      (void) processLine(line);

    readline.addHistory(line);
  }
}

static bool
processLine(const std::string &line)
{
  std::string line1 = CStrUtil::stripSpaces(line);

  if (line1.empty() || line1[0] == '#')
    return true;

  //------

  // get first space separated word
  CParseLine parse(line);

  parse.skipSpace();

  std::string identifier;

  while (parse.isValid()) {
    char c = parse.lookChar();

    if (identifier.empty()) {
      if (! isalpha(c))
        break;
    }
    else {
      if (! isalnum(c) && c != '_')
        break;
    }

    identifier += parse.getChar();
  }

  parse.skipSpace();

  // variable assignment
  if      (identifier != "" && parse.isChar('=')) {
    parse.skipChar();

    parse.skipSpace();

    CExprValuePtr value;

    if (! expr->evaluateExpression(parse.substr(parse.pos()), value))
      return false;

    if (! value.isValid())
      return false;

    expr->createVariable(identifier, value);

    return true;
  }
  // function definition
  else if (identifier != "" && parse.isChar('(')) {
    parse.skipChar();

    parse.skipSpace();

    std::string              arg;
    std::vector<std::string> args;

    while (parse.isValid()) {
      if      (parse.isChar(')')) {
        parse.skipChar();

        break;
      }
      else if (parse.isChar(',')) {
        if (! arg.empty())
          args.push_back(arg);

        arg = "";

        parse.skipChar();
      }
      else
        arg += parse.getChar();
    }

    if (! arg.empty())
      args.push_back(arg);

    parse.skipSpace();

    if (parse.isChar('=')) {
      parse.skipChar();

      parse.skipSpace();

      expr->addFunction(identifier, args, parse.substr(parse.pos()));

      return true;
    }
  }

  //---

  CExprTokenStack pstack = expr->parseLine(line);

  if (function != FUNCTION_PARSE) {
    CExprITokenPtr itoken = expr->interpPTokenStack(pstack);

    if (function != FUNCTION_INTERP) {
      CExprTokenStack cstack = expr->compileIToken(itoken);

      if (function != FUNCTION_COMPILE) {
        CExprValuePtr value;

        if (expr->executeCTokenStack(cstack, value))
          std::cerr << line << " = " << value << std::endl;
        else
          std::cerr << "Error: " << line << std::endl;
      }
    }
  }

  return true;
}
