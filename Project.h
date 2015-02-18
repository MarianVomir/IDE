#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QSet>
#include <QException>

using namespace std;

class Project
{
private:
    QString name;
    QString root;
    QString compiler;
    QString debugger;
    QString compilerFlags;
    QString linkerFlags;
    QString makeUtility;

public:
    const QString& Name() const { return name; }
    const QString& Root() const { return root; }
    const QString& Compiler() const { return compiler; }
    const QString& Debugger() const { return debugger; }
    const QString& CompilerFlags() const { return compilerFlags; }
    const QString& LinkerFlags() const { return linkerFlags; }
    const QString& MakeUtility() const { return makeUtility; }

    explicit Project
    (
        const QString& name,
        const QString& root,
        const QString& compiler,
        const QString& debugger,
        const QString& compilerFlags,
        const QString& linkerFlags,
        const QString& makeUtility
    )
        : name(name),
          root(root),
          compiler(compiler),
          debugger(debugger),
          compilerFlags(compilerFlags),
          linkerFlags(linkerFlags),
          makeUtility(makeUtility)
    {
    }
};

#endif // PROJECT_H
