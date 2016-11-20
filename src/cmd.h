class CMD{
  public:
    uint8_t execute(uint8_t, const char*);


  private:

    void motor_on();
    void motor_off();
    void motor_on_for();
    void motor_on_after();
};
