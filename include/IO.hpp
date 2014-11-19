/*
 * IO.h
 *
 *  Created on: May 7, 2010
 *      Author: fabio
 */

#ifndef IO_H_
#define IO_H_
#include <iostream>

namespace util{
namespace IO{

/**Print an object on an output stream
 * @param object object to print
 * @param os output stream to print on
 * @return os
 */
template<class T> std::ostream& print(const T& object, std::ostream& os=std::cout){
	return os<<object;
}

/**@see print
 * add an end line to print
 */
template<class T> std::ostream& println(const T& object, std::ostream& os=std::cout){
	return print(object,os)<<std::endl;
}

/**Print an STL collection on an output stream. The collection must have the defined type const_iterator
 * and the method begin to get the starting iterator and end to get the ending iterator
 * @param collection collection to print
 * @param ostream stream to print on
 * @return ostream
 */
template<class T> std::ostream& printSTLCollection(const T& collection, std::ostream& os=std::cout, std::string sep=", "){
	typename T::const_iterator it=collection.begin();
	if(collection.begin()==collection.end()) return os<<"[]";
	else{
		os<<"["<<*(it++);
		while(it!=collection.end()) os<<sep<<*it++;
		return os<<"]";
	}
}

/**@see printSTLCollection
 * add an endline to printSTLCollection
 */
template <class T> std::ostream& printlnSTLCollection(const T& collection, std::ostream& os=std::cout, std::string sep=", "){
	return printSTLCollection(collection,os,sep)<<std::endl;
}


/**Print an STL pointer collection on an output stream. The collection must have the defined type const_iterator
 * and the method begin to get the starting iterator and end to get the ending iterator
 * @param collection collection to print
 * @param ostream stream to print on
 * @return ostream
 */
template<class T> std::ostream& printSTLPtrCollection(const T& collection, std::ostream& os=std::cout,std::string sep=", "){
	typename T::const_iterator it=collection.begin();
	if(collection.begin()==collection.end()) return os<<"[]";
	else{
		os<<"["<<**(it++);
		while(it!=collection.end()) os<<sep<<**it++;
		return os<<"]";
	}
}

/**@see printSTLPtrCollection
 * add an endline to printSTLPtrCollection
 */
template <class T> std::ostream& printlnSTLPtrCollection(const T& collection, std::ostream& os=std::cout,std::string sep=", "){
	return printSTLPtrCollection(collection,os,sep)<<std::endl;
}

template <class T,class W> std::ostream& operator<<(std::ostream& os, const std::pair<T,W>& p){
	return os<<"("<<p.first<<","<<p.second<<")";
}


template <class T> std::ostream& printArray(const T *array,int len, std::ostream& os=std::cout,std::string sep=", "){
    if(len==0) return os<<"[]";
    int i=0;
    os<<"["<<array[i++];
    while(i!=len)
        os<<sep<<array[i++];
    return os<<"]";
}

template <class T> std::ostream& printlnArray(const T *array,int len, std::ostream& os=std::cout,std::string sep=", "){
    return printArray(array,len,os,sep)<<std::endl;
}


}

}
#endif /* IO_H_ */
