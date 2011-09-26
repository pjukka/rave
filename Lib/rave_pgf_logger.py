'''
Copyright (C) 2010- Swedish Meteorological and Hydrological Institute (SMHI)

This file is part of RAVE.

RAVE is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RAVE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with RAVE.  If not, see <http://www.gnu.org/licenses/>.
'''
## Management utilities for the logging system.
# Server and receiver functionality are taken almost straight from
# the Python documentation, with slightly better security. 

## @file
## @author Daniel Michelson, SMHI
## @date 2010-07-24

import sys, os, struct
import cPickle
import logging
import logging.handlers
import SocketServer
from types import StringType, UnicodeType
from rave_defines import PGF_HOST, LOGFILE, LOGFILESIZE, LOGFILES, LOGPORT, LOGPIDFILE, STDOE
from rave_daemon import Daemon


## Initializes the system logger.
# @param logger an instance returned by \ref logging.getLogger()
def init_logger(logger):
    logger.setLevel(logging.INFO)
    handler = logging.handlers.RotatingFileHandler(LOGFILE,
                                                   maxBytes = LOGFILESIZE,
                                                   backupCount = LOGFILES)
    # This formatter removes the fractions of a second in the time.
#     formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s',
#                                   '%Y-%m-%d %H:%M:%S %Z')
    # The default formatter contains fractions of a second in the time.
    formatter = logging.Formatter('%(asctime)-15s %(levelname)-7s %(name)-8s: %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.info("Logging system initialized. Starting...")


## Handler for a streaming logging request.
#  This basically logs the record using whatever logging policy is
#  configured locally.
class LogRecordStreamHandler(SocketServer.StreamRequestHandler):

    ## Handle multiple requests - each expected to be a 4-byte length,
    # followed by the LogRecord in pickle format. Logs the record
    # according to whatever policy is configured locally.
    def handle(self):

        while 1:
            chunk = self.connection.recv(4)
            if len(chunk) < 4:
                break
            slen = struct.unpack(">L", chunk)[0]
            chunk = self.connection.recv(slen)
            while len(chunk) < slen:
                chunk = chunk + self.connection.recv(slen - len(chunk))
            obj = self.unPickle(chunk)
            # Rudimentary security, to overcome the vulnerability of pickling
            if type(obj['msg']) in (StringType, UnicodeType):
                record = logging.makeLogRecord(obj)
                self.handleLogRecord(record)

    ## Unpacks whatever comes in over the wire.
    # @param data payload in pickle format
    # @return unpacked payload in native Python format
    def unPickle(self, data):
        return cPickle.loads(data)


    ## Handles a log record
    # @param record log record
    def handleLogRecord(self, record):
        # if a name is specified, we use the named logger rather than the one
        # implied by the record.
        if self.server.logname is not None:
            name = self.server.logname
        else:
            name = record.name
        logger = logging.getLogger(name)
        # N.B. EVERY record gets logged. This is because Logger.handle
        # is normally called AFTER logger-level filtering. If you want
        # to do filtering, do it at the client end to save wasting
        # cycles and network bandwidth!
        logger.handle(record)


## Simple TCP socket-based logging receiver suitable for testing, and maybe a little more.
class LogRecordSocketReceiver(SocketServer.ThreadingTCPServer):
    allow_reuse_address = 1

    ## Initializer
    # @param host string host name
    # @param port int port number
    # @param handler server instance 
    def __init__(self, host=PGF_HOST, port=LOGPORT,
                 handler=LogRecordStreamHandler):
        SocketServer.ThreadingTCPServer.__init__(self, (host, port), handler)
        self.abort = 0
        self.timeout = 1
        self.logname = None

    ## Serve until stopped
    def serve_until_stopped(self):
        import select
        abort = 0
        while not abort:
            rd, wr, ex = select.select([self.socket.fileno()],
                                       [], [],
                                       self.timeout)
            if rd:
                self.handle_request()
            abort = self.abort


class rave_pgf_logger_server(Daemon):

  ## Constructor
  # @param host URI to the host for this server.
  # @param port int port number to the host for this server.
  # @param stdin string path to where to direct stdin
  # @param stdout string path to where to direct stdout
  # @param stderr string path to where to direct stderr
  def __init__(self, host=PGF_HOST, port=LOGPORT,
#               stdin='/dev/null', stdout=STDOE, stderr=STDOE):
               stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
    self.pidfile = LOGPIDFILE
    self.stdin = stdin
    self.stderr = stdout
    self.stdout = stderr
    self.host = host
    self.port = port


  ## Determines whether the server is running or not.
  def status(self):
      if os.path.isfile(self.pidfile):
          fd = open(self.pidfile)
          c = fd.read()
          fd.close()
          try:
              pgid = os.getpgid(int(c))
              return "running with PID %s and GID %i" % (c[:-1], pgid) 
          except:
              return "not running"
      else:
          return "not running"


  ## Runs the server.
  # Creates an instance of a LogRecordSocketReceiver and serves it.
  # Note that the start(), stop(), and restart() methods are inherited from
  # \ref Daemon , but you can call fg() to run the server in the
  # foreground, ie. not daemonize, which is useful for debugging.
  def run(self):
    #import atexit
    self.server = LogRecordSocketReceiver(host=self.host, port=self.port)
    #atexit.register(self.server.instance._dump_queue)
    self.server.serve_until_stopped()

        

if __name__ == "__main__":
  prog = "rave_pgf_logger_server"
  usage = "usage: %s start|stop|status|restart|fg" % prog

  if len(sys.argv) != 2:
    print usage
    sys.exit()

  ARG = sys.argv[1].lower()

  if ARG not in ('start', 'stop', 'status', 'restart', 'fg'):
    print usage
    sys.exit()

  this = rave_pgf_logger_server()

  if ARG == 'stop':
    this.stop()

  if ARG == 'start':
    this.start()

  if ARG == 'restart':
    this.restart()

  if ARG == 'status':
    print "%s is %s" % (prog, this.status())

  if ARG == 'fg':
    this.run()

