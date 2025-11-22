#ifndef BCRUNTIME_BCKEYWORDS_H
#define BCRUNTIME_BCKEYWORDS_H

#define $VAR __auto_type
#define $LET const __auto_type
#define $TYPE(_type_) typeof(_type_)
#define $TLS _Thread_local

#define $PROCESS_DESTRUCTOR __attribute__((destructor))
#define $PROCESS_CONSTRUCTOR __attribute__((constructor))

#endif //BCRUNTIME_BCKEYWORDS_H