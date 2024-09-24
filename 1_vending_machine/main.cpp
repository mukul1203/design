#include <vector>
#include <unordered_map>
#include <string>
#include <future>

//--------------------------------------------------------------------//
struct item
{
    using id = int;
    enum type : int
    {
        invalid = -1,
        grocery,
        food,
        cosmetic,
    };
    id _id{-1};
    long double _price{0.0};
    type _type{invalid};
};

using inventory = std::unordered_map<item::id, std::pair<int, item>>;
//--------------------------------------------------------------------//
struct inventory_manager
{
    inventory get() const
    {
        return _inventory;
    }
    void add(item it, int count)
    {
        auto &[count, item] = _inventory[it._id];
        ++count;
        item = it;
    }
    void remove(const item::id &id, int count)
    {
        if (_inventory.count(id) == 0 || _inventory[id].first < count)
        {
            throw std::runtime_error("Not enough items in inventory!");
        }
        _inventory[id].first -= count;
        if (_inventory[id].first == 0)
        {
            _inventory.erase(id);
        }
    }

protected:
    inventory _inventory{};
};

//--------------------------------------------------------------------//
struct basket_manager : public inventory_manager
{
    long double total_cost() const
    {
        long double res = 0;
        for (const auto &[id, entry] : _inventory)
        {
            const auto &[count, it] = entry;
            res += it._price * count;
        }
        return res;
    }
};

//--------------------------------------------------------------------//

struct payment_option
{
    virtual ~payment_option() {}
};

struct upi : public payment_option
{
};

struct card : public payment_option
{
};

struct payment_service
{
    virtual std::future<bool> pay(long double amount, const payment_option &po) = 0;
};

struct upi_payment_service : public payment_service
{
    std::future<bool> pay(long double amount, const payment_option &po) {}
};

struct card_payment_service : public payment_service
{
    std::future<bool> pay(long double amount, const payment_option &po) {}
};

upi_payment_service &get_upi_payment_service()
{
    static auto service = upi_payment_service();
    return service;
}

card_payment_service &get_card_payment_service()
{
    static auto service = card_payment_service();
    return service;
}

struct payment_service_factory
{
    static payment_service &get(payment_option *po)
    {
        if (dynamic_cast<upi *>(po))
        {
            return get_upi_payment_service();
        }
        else if (dynamic_cast<card *>(po))
        {
            return get_card_payment_service();
        }
    }
};

struct payment_manager
{
    std::future<bool> pay(long double amount, payment_option &po)
    {
        return payment_service_factory::get(&po).pay(amount, po);
    }
};

//--------------------------------------------------------------------//

struct item_dispenser
{
    std::future<bool> dispense(item::id id, int count, int slot)
    {
        // call machine drivers to open the required slot and release the count items
    }
};

//--------------------------------------------------------------------//

struct vending_machine
{
    inventory get_inventory() const
    {
        return _inventory_mgr.get();
    }
    void add_to_basket(item::id id, int count)
    {
        auto inv = get_inventory();
        auto &[inv_count, item] = inv[id];
        if (count > inv_count)
        {
            throw std::runtime_error("Not enough inventory");
        }
        _basket_mgr.add(item, count);
        _inventory_mgr.remove(id, count);
    }

    void remove_from_basket(item::id id, int count)
    {
        auto inv = get_inventory();
        auto &[inv_count, item] = inv[id];
        auto &[bk_count, item] = _basket_mgr.get()[id];
        if (count > bk_count)
        {
            throw std::runtime_error("Basket has lesser items than to be removed");
        }
        _basket_mgr.remove(id, count);
        _inventory_mgr.add(item, count);
    }

    std::vector<payment_option> get_payment_options() const
    {
        return {upi(), card()};
    }

    void checkout(payment_option po)
    {
        auto pay_fut = _payment_mgr.pay(_basket_mgr.total_cost(), po);
        if (pay_fut.get())
        {
            for (const auto &[id, entry] : _basket_mgr.get())
            {
                auto &[count, item] = entry;
                _dispenser.dispense(id, count, _item_metadata[id].slot_idx);
            }
        }
    }

private:
    struct item_metadata
    {
        std::string name{"unknown"};
        std::uint8_t slot_idx{0};
    };
    std::unordered_map<item::id, item_metadata> _item_metadata;
    inventory_manager _inventory_mgr;
    basket_manager _basket_mgr;
    item_dispenser _dispenser;
    payment_manager _payment_mgr;
};