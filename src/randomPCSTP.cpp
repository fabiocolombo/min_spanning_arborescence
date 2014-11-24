#include <iostream>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace boost;
using namespace boost::random;

int main(int argc, char** argv){
	if(argc != 3){
		cerr<<"Error, usage "<<argv[0]<<" number_of_nodes pprob"<<endl;
		return EXIT_FAILURE;
	}
	int n=lexical_cast<int>(argv[1]);
	double pprob=lexical_cast<double>(argv[2]);
	mt19937 gen;
	gen.seed(time(0));
	uniform_int<> cdist(1,10);
	uniform_int<> pdist(0,20);
	bernoulli_distribution<> pprobdist(pprob);

	cout<<"Radice: 1"<<endl;
	cout<<"NumVertici: "<<n<<endl;
	cout<<"NumArchi: "<<((n)*(n-1)/2)<<endl;

	cout<<"Costi:"<<endl;
	for(int i=1;i<=n;++i){
		for(int j=i+1;j<=n;++j){
			cout<<i<<" "<<j<<" "<<cdist(gen)<<endl;
		}
	}

	cout<<"Premi:"<<endl;
	for(int i=1;i<=n;++i){
		if(pprobdist(gen)){
			cout<<i<<" "<<0<<endl;
		}
		else{
			cout<<i<<" "<<pdist(gen)<<endl;
		}
	}
	cout<<"Pesi:"<<endl;
	for(int i=1;i<=n;++i){		
		cout<<i<<" "<<1<<endl;
	}
	return EXIT_SUCCESS;
}