#ifndef ILO_GROWABLE_MATRIX_HPP
#define ILO_GROWABLE_MATRIX_HPP
#include "ilcplex/ilocplex.h"
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <sstream>
#include <string>
//#include <boost/functional/hash/hash.hpp>
//#include "hash.hpp"

namespace util{
	namespace iloGrowableMatrix{

		template<typename T> class IloStringIndexedMap{
		public:
			typedef std::string KEY;
			typedef boost::unordered_map<KEY,T> VarMap;
		private:
			IloEnv& env;
			double lb,ub;
			VarMap map;
			std::string baseName;	
		public:
			IloStringIndexedMap(IloEnv& env,double lb, double ub, std::string baseName):
				  env(env),
			      lb(lb),
				  ub(ub),
				  baseName(baseName)
			  {}

			  const VarMap& getVarMap(){
			      return map;
			  }

			  T& operator()(const std::string& name){
				  KEY k(name);
				  typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
				  if(it==map.end()){
#ifndef NDEBUG
					  std::stringstream sstream;
					  sstream<<baseName<<"["<<name<<"]";
					  it=map.insert(std::pair<KEY,T>(k,T(env,lb,ub,sstream.str().c_str()))).first;
#else
					  it=map.insert(std::pair<KEY,T>(k,T(env,lb,ub))).first;
#endif
				  }

				  return it->second;
			  }

			  void remove(const std::string& name){
			      KEY k(name);
			      typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
			      if(it!=map.end()){
			          it->second.end();
			          map.erase(it);
			      }
			  }

			  std::ostream& printState(IloCplex& solver, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex& solver, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(solver.getValue(it->second),0.0))
						os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

              std::ostream& printState(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(cb.getValue(it->second),0.0))
						os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

			  void clear(){
			      for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
				it->second.end();
			      }
			      map.clear();
			  }		
		};

		struct ht2{
			std::size_t operator()(const boost::tuple<int,int>& t) const{
				std::size_t seed = 0;
				boost::hash_combine(seed, boost::get<0>(t));
				boost::hash_combine(seed, boost::get<1>(t));
				return seed;
			}
		};

		template<typename T> class IloBiIndexedMap{
        public:
			typedef boost::tuple<int,int> KEY;
			typedef boost::unordered_map<KEY,T,ht2> VarMap;
        private:
			IloEnv& env;
			double lb,ub;
			VarMap map;
			std::string baseName;
		public:
			IloBiIndexedMap(IloEnv& env,double lb, double ub, std::string baseName):
				  env(env),
			      lb(lb),
				  ub(ub),
				  baseName(baseName)
			  {}

			  const VarMap& getVarMap(){
			      return map;
			  }

			  T& operator()(int i, int j){
				  KEY k(i,j);
				  typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
				  if(it==map.end()){
// #ifndef NDEBUG
					  std::stringstream sstream;
					  sstream<<baseName<<"["<<i<<","<<j<<"]";
					  it=map.insert(std::pair<KEY,T>(k,T(env,lb,ub,sstream.str().c_str()))).first;
// #else
// 					  it=map.insert(std::pair<KEY,T>(k,T(env,lb,ub))).first;
// #endif
				  }

				  return it->second;
			  }

			  void remove(int i, int j){
			      KEY k(i,j);
			      typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
			      if(it!=map.end()){
			          it->second.end();
			          map.erase(it);
			      }
			  }

			  std::ostream& printState(IloCplex& solver, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex& solver, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(solver.getValue(it->second),0.0))
						os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

              std::ostream& printState(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(cb.getValue(it->second),0.0))
						os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

			  void clear(){
			      for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
				it->second.end();
			      }
			      map.clear();
			  }
		};

		template<typename T> class IloIndexedMap{
            public:
			typedef int KEY;
			typedef boost::unordered_map<KEY,T> VarMap;

			private:
			VarMap map;
			IloEnv& env;
			double lb,ub;
			std::string baseName;
		public:
			IloIndexedMap(IloEnv& env,double lb, double ub, std::string baseName):
				env(env),
				lb(lb),
				ub(ub),
				baseName(baseName)
			  {}

			  const VarMap& getVarMap(){
			      return map;
			  }

			  T& operator()(int i){
				  typename boost::unordered_map<KEY, T>::iterator it=map.find(i);
				  if(it==map.end()){
					  it=map.insert(std::pair<KEY,T>(i,T(env,lb,ub))).first;
					  it->second.setName((baseName+"["+boost::lexical_cast<std::string>(i)+"]").c_str());
				  }
				  return it->second;
			  }

              void remove(int i){
			      KEY k(i);
			      typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
			      if(it!=map.end()){
			          it->second.end();
			          map.erase(it);
			      }
			  }

			  std::ostream& printState(IloCplex& solver, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex& solver, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(solver.getValue(it->second),0.0))
						os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

			  void clear(){
			      map.clear();
			  }

              std::ostream& printState(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(cb.getValue(it->second),0.0))
						os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }
		};

		struct ht3{
			std::size_t operator()(const boost::tuple<int,int,int>& t) const{
				std::size_t seed = 0;
				boost::hash_combine(seed, boost::get<0>(t));
				boost::hash_combine(seed, boost::get<1>(t));
				boost::hash_combine(seed, boost::get<2>(t));
				return seed;
			}
		};

		template<typename T> class IloTriIndexedMap{
            public:
			typedef boost::tuple<int,int,int> KEY;
			typedef boost::unordered_map<KEY,T,ht3> VarMap;

			private:
			double lb,ub;
			VarMap map;
			IloEnv& env;
			std::string baseName;
		public:
			IloTriIndexedMap(IloEnv& env,double lb, double ub,std::string baseName):
			  env(env),
				  lb(lb),
				  ub(ub),
				  baseName(baseName)
			  {}

            const VarMap& getVarMap(){
			      return map;
			}

			  T& operator()(int i, int j,int l){
				  KEY k(i,j,l);
				  typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
				  if(it==map.end()){
					  it=map.insert(std::pair<KEY,T>(k,T(env,lb,ub))).first;
					  it->second.setName((baseName+"["+boost::lexical_cast<std::string>(i)+","+boost::lexical_cast<std::string>(j)+","+boost::lexical_cast<std::string>(l)+"]").c_str());
				  }
				  return it->second;
			  }

              void remove(int i, int j, int l){
			      KEY k(i,j,l);
			      typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
			      if(it!=map.end()){
			          it->second.end();
			          map.erase(it);
			      }
			  }

			  std::ostream& printState(IloCplex& solver, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex& solver, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(solver.getValue(it->second),0.0))
						os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

			  void clear(){
			      map.clear();
			  }

              std::ostream& printState(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(cb.getValue(it->second),0.0))
						os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }
		};

		struct ht4{
			std::size_t operator()(const boost::tuple<int,int,int,int>& t) const{
				std::size_t seed = 0;
				boost::hash_combine(seed, boost::get<0>(t));
				boost::hash_combine(seed, boost::get<1>(t));
				boost::hash_combine(seed, boost::get<2>(t));
				boost::hash_combine(seed, boost::get<3>(t));
				return seed;
			}
		};

        template<typename T> class IloQuadIndexedMap{
            public:
			typedef boost::tuple<int,int,int,int> KEY;
			typedef boost::unordered_map<KEY,T,ht4> VarMap;

			private:
			double lb,ub;
			VarMap map;
			IloEnv& env;
			std::string baseName;
		public:
			IloQuadIndexedMap(IloEnv& env,double lb, double ub,std::string baseName):
			  env(env),
				  lb(lb),
				  ub(ub),
				  baseName(baseName)
			  {}

            const VarMap& getVarMap(){
			      return map;
			}

			  T& operator()(int i, int j,int l,int q){
				  KEY k(i,j,l,q);
				  typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
				  if(it==map.end()){
					  it=map.insert(std::pair<KEY,T>(k,T(env))).first;
					  it->second.setName((baseName+"["+boost::lexical_cast<std::string>(i)+","+boost::lexical_cast<std::string>(j)+","+boost::lexical_cast<std::string>(l)+"]").c_str());
				  }
				  return it->second;
			  }

              void remove(int i, int j,int l,int q){
			      KEY k(i,j,l,q);
			      typename boost::unordered_map<KEY, T>::iterator it=map.find(k);
			      if(it!=map.end()){
			          it->second.end();
			          map.erase(it);
			      }
			  }

			  std::ostream& printState(IloCplex& solver, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex& solver, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(solver.getValue(it->second),0.0))
						os<<it->second<<"="<<solver.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }

			  void clear(){
			      map.clear();
			  }

              std::ostream& printState(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout){
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it)
					  os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  return os;
			  }

			  std::ostream& printStatePositive(IloCplex::ControlCallbackI& cb, std::ostream& os=std::cout) const{
				  for(typename boost::unordered_map<KEY,T>::const_iterator it=map.begin();it!=map.end();++it){
					  if(gr(cb.getValue(it->second),0.0))
						os<<it->second<<"="<<cb.getValue(it->second)<<std::endl;
				  }
				  return os;
			  }
		};

	}
}

#endif
