import logging

logger_format = '%(asctime)-15s %(levelname)s %(message)s'


def setup_logger(logname: str)->None:
    logging.basicConfig(level=logging.DEBUG, format=logger_format, filename=logname, filemode='a')

    # only for debug purpose
    fh = logging.StreamHandler()
    fh.setLevel(logging.DEBUG)
    logging.getLogger().addHandler(fh)


def get_logger()->logging.Logger:
    # make sure user gets the already configured logger
    return logging.getLogger()