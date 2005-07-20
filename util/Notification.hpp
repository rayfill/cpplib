#ifndef NOTIFICATION_HPP_
#define NOTIFICATION_HPP_

#include <exception>
#include <stdexcept>
#include <list>

class Observable;

/**
 * Observable�N���X����̒ʒm���󂯎�邱�Ƃ�錾����C���^�t�F�[�X
 */
class Observer
{
	friend class Observable;
protected:
public:
	/**
	 * �ʒm�󂯎��̂��߂̃C���^�t�F�[�X
	 * @arg notifier �ʒm�� Observable �N���X�̃|�C���^
	 */
	virtual void notify(Observable* notifier) = 0;

	/**
	 * �f�t�H���g�f�X�g���N�^
	 */
	virtual ~Observer() {}

};

/**
 * Observable �N���X�ɒʒm������N���X����������C���^�t�F�[�X
 */
class Observable
{
private:
	typedef std::list<Observer*> ServerList;

	/**
	 * �ʒm���󂯎�� Observer �N���X�̃��X�g
	 */
	ServerList servers;

protected:
	/**
	 * �����Ŏg�p���錟���p�t�@���N�^
	 */
	class Finder : public std::unary_function<Observer*, bool>
	{
	private:
		/**
		 * �����Ώۂ� Observer �N���X�̃|�C���^
		 */
		const Observer* server;

		/**
		 * �����s�\�̂��߂̃v���C�x�[�g�f�t�H���g�R���X�g���N�^
		 */
		Finder() throw() {}

	protected:
		/**
		 * �����g�p�̂��߂̃R�s�[�R���X�g���N�^
		 */
		Finder(const Observer* server_) throw(): server(server_) {}

	public:
		/**
		 * �t�@�N�g�����\�b�h
		 * @arg �����ΏۂƂȂ� Observer �N���X�̃|�C���^
		 */
		static Finder create(Observer* server_) throw()
		{
			Finder result(server_);
			return result;
		}

		/**
		 * �t�@���N�^���\�b�h
		 * @arg target ��r�ΏۂƂȂ� Observer �N���X�̃|�C���^
		 * @return ��r����. �������ꍇ:true, ����ȊO:false
		 */
		bool operator()(Observer* target) throw()
		{
			return target == server;
		}
	};

public:
	/**
	 * �R���X�g���N�^
	 */
	Observable() throw():
		servers()
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~Observable() {}

	/**
	 * �ʒm�̎��s
	 */
	void update()
	{
		for(ServerList::iterator itor = servers.begin();
			itor != servers.end();
			++itor)
		{
			(*itor)->notify(this);
		}
	}

	/**
	 * Observer �̃A�^�b�`
	 * @arg server �ڑ����� Observer �N���X�̃|�C���^
	 * @exception std::bad_alloc() ������������Ȃ������ꍇ
	 */
	void attachObserver(Observer* server) throw(std::bad_alloc)
	{
		servers.push_back(server);
	}

	/**
	 * �I�u�U�[�o�̐ؒf
	 * @arg detachServer �ؒf���� Observer �N���X�̃|�C���^
	 */
	void detachObserver(Observer* detachServer) throw()
	{
		servers.remove_if(Finder::create(detachServer));
	}

	/**
	 * �ڑ�����Ă���N���C�A���g�̐��̎擾
	 */
	const size_t serverCount() throw()
	{
		return servers.size();
	}
};

#endif /* NOTIFICATION_HPP_ */
