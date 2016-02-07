
#include <algorithm>
#include "manager.h"
#include "log.h"
#include "module.h"
#include "defs.h"
#include <thread>
#include <chrono>

Manager* Manager::msgMan = nullptr;

void Manager :: start()
 { LOG_INFO("Manager: Start threads.");
   lock();
   for(auto it : threads)
    it->start();
   unlock();
 }

bool Manager :: clear()
 { threads.erase(std::remove_if(threads.begin(), threads.end(),[](Thread* thr)->bool
    { if(thr->isStatus() == false)
       {delete thr; return true;}
      else return false;
    }), threads.end());

   int n = threads.size();
   if(n == 0) return false;
   else return true;
 }

unsigned int Manager :: add_thread(Thread* thr)
 { threads.push_back(thr);
   return thr->getId();
 }

void Manager :: add_module(Module* mdl)
 { unsigned int i = mdl->getThreadId();
   auto thr = std::find_if(threads.begin(), threads.end(), [i] (Thread* thr)->bool
    { if(i == thr->getId()) return true;
      else return false;
    });

   (*thr)->addModule(mdl);
   mdl->setNumQueue(create_queue(*thr));
   mdl->connectMsg();
   LOG_INFO("Manager: Registration module \"%s\"", mdl->getName());
 }

int Manager :: create_queue(Thread* thr)
 { QModule* tmp = new QModule;

   tmp->active = false;
   tmp->thr = thr;
   qModule.push_back(tmp);
   return qModule.size() - 1;
 }

void Manager :: reg_message(unsigned int MsgId, unsigned int NumQueue)
 { std::multimap<unsigned int, unsigned int>::iterator it;
   auto ret = msgMan->msgTab.equal_range(MsgId);
   for(it = ret.first; it != ret.second; ++it)
    if(it->second == NumQueue) break;

   if(it == ret.second) msgMan->msgTab.emplace(MsgId, NumQueue);
  }

bool Manager :: handlerMsg(int msg, Object* item, int param)
 { switch(msg)
    { case MSG_DEACTIVE:
       if(param < (int)qModule.size())
        { if((qModule[param]->active == true)&&(qModule[param]->qmes.size() == 0))
           { qModule[param]->active = false;
             qModule[param]->thr->deactive();
           }
        }
       else LOG_ERROR("Manager: Invalid parameter in the message MSG_DEACTIVE.");
       return false;
      case MSG_ACTIVE:
       if(param < (int)qModule.size())
        { if(qModule[param]->active == false)
           { qModule[param]->active = true;
             qModule[param]->thr->active();
           }
        }
       else LOG_ERROR("Manager: Invalid parameter in the message MSG_ACTIVE.");
       return false;
      case MSG_CONNECT:
       if(item->isSuperClass(Module::Instance))
        reg_message((unsigned int)param, ((Module*)item)->getNumQueue());
       else LOG_ERROR("Manager: Invalid parameter in the message MSG_CONNECT.");
       return false;
    }
   return true;
 }

int Manager :: put(int msg, Object* item, int param)
 { int i = 0;
   Message stMes;

   if(handlerMsg(msg, item, param) == false) return -1;
   auto ret = msgTab.equal_range(msg);
   if(ret.first == msgTab.end()) return 0;

   for(auto it = ret.first; it != ret.second; ++it)
    { if(item) stMes.data = item->retain();
      else stMes.data = nullptr;
      stMes.hValue = msg;
      stMes.Param = param;
      qModule[it->second]->qmes.push(stMes);
      if(qModule[it->second]->active == false)
       { qModule[it->second]->active = true;
         qModule[it->second]->thr->active();
       }
      i++;
    }
   return i;
 }

bool Manager :: get(unsigned key, int &msg, Object* &obj, int &param)
 { Message message;

   if((key < qModule.size())&&(!qModule[key]->qmes.empty()))
    { message = qModule[key]->qmes.front();
      qModule[key]->qmes.pop();
      msg = message.hValue;
      param = message.Param;
      obj = message.data;
      return true;
    }
   return false;
 }

// ----------------------------------------------------------------------

bool Manager :: Init()
 { if(msgMan == nullptr) msgMan = new Manager();
   else
    { LOG_WARNING("Manager: Attempt to initialize the secondary.");
      return true;
    }
   if(msgMan == nullptr)
    { LOG_CRIT("Manager: Not init.");
      return false;
    }
   LOG_INFO("Manager: Init.");
   return true;
 }

void Manager :: Clear()
 { bool bl;
   while(true)
    { msgMan->lock();
      bl = msgMan -> clear();
      msgMan->unlock();
      if(bl) std::this_thread::sleep_for(std::chrono::milliseconds(100));
      else break;
    }
   LOG_INFO("Manager: Clear.");
   delete msgMan;
 }

unsigned int Manager :: addThread(Thread* thr)
 { int n;
   msgMan->lock();
   n = msgMan -> add_thread(thr);
   msgMan->unlock();
   return n;
 }

void Manager :: addModule(Module* mdl)
 { msgMan->add_module(mdl); }
