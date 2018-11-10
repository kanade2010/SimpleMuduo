#ifndef _AUTO_PTR_HH
#define _AUTO_PTR_HH

namespace oneself{

template<typename T>
class auto_ptr{
public:
	explicit auto_ptr(T* p = 0):m_ptr(p){
	}
	
	auto_ptr(auto_ptr& obj):m_ptr(obj.release()){
	}
	
	auto_ptr& operator=(auto_ptr& obj){
		reset(obj.release());
		return *this;
	}
	
	~auto_ptr(){
		delete m_ptr;
	}

	T* release(){
		T* tmp = m_ptr;
		m_ptr = 0;
		return tmp;
	}
	
	void reset(T* p){
		if(m_ptr != p)
			delete m_ptr;
		m_ptr = p;
	}
	
	T* get() const {
		return m_ptr;
	}
	
	T* operator->(){
		return get();
	}
	
	T& operator*(){
		return *get();
	}

private:
	T* m_ptr;
};

}
#endif