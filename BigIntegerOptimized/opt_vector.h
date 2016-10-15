#ifndef OPT_VECTOR_H
#define OPT_VECTOR_H

#include <memory>
#include <vector>

struct opt_vector
{
	opt_vector();//done
	
	opt_vector(size_t size);//done
	
	opt_vector(opt_vector const& other);//done

	opt_vector& operator=(opt_vector const& other);//done
	
	~opt_vector();//done

	void clear();//done

	void push_back(unsigned el);//done

	void pop_back();//done
	
	void resize(size_t size);//done

	size_t size() const;//done

	unsigned const& operator[](int i) const;//done

	unsigned& operator[](int i);//done

	unsigned const& back() const;//done

	friend bool operator==(opt_vector const& a, opt_vector const& b);//done

	friend void reverse(opt_vector& a);//done

private:
	size_t sz;
	std::shared_ptr<std::vector<unsigned>> v;
	unsigned n;

	void makeCopy();//done
};

#endif //OPT_VECTOR_H