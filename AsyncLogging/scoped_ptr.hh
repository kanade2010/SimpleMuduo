#ifndef _SCOPE_PTR_HH
#define _SCOPE_PTR_HH
//  scoped_ptr mimics a built-in pointer except that it guarantees deletion
//  of the object pointed to, either on destruction of the scoped_ptr or via
//  an explicit reset(). scoped_ptr is a simple solution for simple needs;
//  use shared_ptr or std::auto_ptr if your needs are more complex.

/*
scoped_ptr 是局部智能指针 不允许转让所有权。
*/
template <class T>
class scoped_ptr
{
public:
	scoped_ptr(T *p = 0) :m_ptr(p) {
	}
	
	~scoped_ptr(){
		delete m_ptr;
	}
	
	T&operator*() const {
		return *m_ptr;
	}
	
	T*operator->() const {
		return m_ptr;
	}
	
	void reset(T *p)//拥有权不允许转让  但是可以让智能指针指向另一个空间  
	{
		if (p != m_ptr && m_ptr != 0)
			delete m_ptr;
		m_ptr = p;
	}

	T* get() const {
		return m_ptr;
	}

	operator bool() const { get() != NULL; }

private://将拷贝构造和赋值  以及判等判不等  都设置为私有方法
	//对象不再能调用，即不能拷贝构造和赋值  也就达到了不让转移拥有权的目的
	scoped_ptr(const scoped_ptr<T> &y);
	const scoped_ptr<T> operator=(const scoped_ptr<T> &);
	void operator==(scoped_ptr<T> const &) const;
	void operator!=(scoped_ptr<T> const &) const;

	T* m_ptr;
};

#endif